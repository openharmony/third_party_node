#include <algorithm>
#include <atomic>
#include <climits> // INT_MAX
#include <cmath>
#include <cstring>
#include <unistd.h>

#include "v8-debug.h"
#include "v8-internal.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"
#include "v8-statistics.h"
#include "v8-version-string.h"
#define JSVM_EXPERIMENTAL
#include "js_native_api_v8.h"
#include "jsvm.h"
#include "jsvm_env.h"
#include "jsvm_log.h"
#include "jsvm_reference.h"
#include "jsvm_util.h"
#include "libplatform/libplatform.h"
#include "platform/platform.h"
#include "sourcemap.def"

namespace v8impl {

namespace {

enum IsolateDataSlot {
    kIsolateData = 0,
    kIsolateSnapshotCreatorSlot = 1,
};

enum ContextEmbedderIndex {
    kContextEnvIndex = 1,
};

struct IsolateData {
    IsolateData(v8::StartupData* blob) : blob(blob) {}

    ~IsolateData()
    {
        delete blob;
    }

    v8::StartupData* blob;
    v8::Eternal<v8::Private> typeTagKey;
    v8::Eternal<v8::Private> wrapperKey;
};

static void CreateIsolateData(v8::Isolate* isolate, v8::StartupData* blob)
{
    auto data = new IsolateData(blob);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    if (blob) {
        // NOTE: The order of getting the data must be consistent with the order of
        // adding data in OH_JSVM_CreateSnapshot.
        auto wrapperKey = isolate->GetDataFromSnapshotOnce<v8::Private>(0);
        auto typeTagKey = isolate->GetDataFromSnapshotOnce<v8::Private>(1);
        data->wrapperKey.Set(isolate, wrapperKey.ToLocalChecked());
        data->typeTagKey.Set(isolate, typeTagKey.ToLocalChecked());
    } else {
        data->wrapperKey.Set(isolate, v8::Private::New(isolate));
        data->typeTagKey.Set(isolate, v8::Private::New(isolate));
    }
    isolate->SetData(v8impl::kIsolateData, data);
}

static IsolateData* GetIsolateData(v8::Isolate* isolate)
{
    auto data = isolate->GetData(v8impl::kIsolateData);
    return reinterpret_cast<IsolateData*>(data);
}

static void SetIsolateSnapshotCreator(v8::Isolate* isolate, v8::SnapshotCreator* creator)
{
    isolate->SetData(v8impl::kIsolateSnapshotCreatorSlot, creator);
}

static v8::SnapshotCreator* GetIsolateSnapshotCreator(v8::Isolate* isolate)
{
    auto data = isolate->GetData(v8impl::kIsolateSnapshotCreatorSlot);
    return reinterpret_cast<v8::SnapshotCreator*>(data);
}

static void SetContextEnv(v8::Local<v8::Context> context, JSVM_Env env)
{
    context->SetAlignedPointerInEmbedderData(kContextEnvIndex, env);
}

static JSVM_Env GetContextEnv(v8::Local<v8::Context> context)
{
    auto data = context->GetAlignedPointerFromEmbedderData(kContextEnvIndex);
    return reinterpret_cast<JSVM_Env>(data);
}

class OutputStream : public v8::OutputStream {
public:
    OutputStream(JSVM_OutputStream stream, void* data, int chunkSize = 65536)
        : stream(stream), streamData(data), chunkSize(chunkSize)
    {}

    int GetChunkSize() override
    {
        return chunkSize;
    }

    void EndOfStream() override
    {
        stream(nullptr, 0, streamData);
    }

    WriteResult WriteAsciiChunk(char* data, const int size) override
    {
        return stream(data, size, streamData) ? kContinue : kAbort;
    }

private:
    JSVM_OutputStream stream;
    void* streamData;
    int chunkSize;
};

static std::unique_ptr<v8::Platform> g_platform = v8::platform::NewDefaultPlatform();

static std::vector<intptr_t> externalReferenceRegistry;

static std::unordered_map<std::string, std::string> sourceMapUrlMap;

static std::unique_ptr<v8::ArrayBuffer::Allocator> defaultArrayBufferAllocator;

static v8::ArrayBuffer::Allocator* GetOrCreateDefaultArrayBufferAllocator()
{
    if (!defaultArrayBufferAllocator) {
        defaultArrayBufferAllocator.reset(v8::ArrayBuffer::Allocator::NewDefaultAllocator());
    }
    return defaultArrayBufferAllocator.get();
}

static void SetFileToSourceMapMapping(std::string&& file, std::string&& sourceMapUrl)
{
    auto it = sourceMapUrlMap.find(file);
    if (it == sourceMapUrlMap.end()) {
        sourceMapUrlMap.emplace(file, sourceMapUrl);
        return;
    }
    auto&& prevSourceMapUrl = it->second;
    CHECK(prevSourceMapUrl == sourceMapUrl);
}

static std::string GetSourceMapFromFileName(std::string&& file)
{
    auto it = sourceMapUrlMap.find(file);
    if (it != sourceMapUrlMap.end()) {
        return it->second;
    }
    return "";
}

template<typename CCharType, typename StringMaker>
JSVM_Status NewString(JSVM_Env env, const CCharType* str, size_t length, JSVM_Value* result, StringMaker stringMaker)
{
    CHECK_NEW_STRING_ARGS(env, str, length, result);

    auto isolate = env->isolate;
    auto strMaybe = stringMaker(isolate);
    CHECK_MAYBE_EMPTY(env, strMaybe, JSVM_GENERIC_FAILURE);
    *result = v8impl::JsValueFromV8LocalValue(strMaybe.ToLocalChecked());
    return ClearLastError(env);
}

inline JSVM_Status V8NameFromPropertyDescriptor(JSVM_Env env,
                                                const JSVM_PropertyDescriptor* p,
                                                v8::Local<v8::Name>* result)
{
    if (p->utf8name != nullptr) {
        CHECK_NEW_FROM_UTF8(env, *result, p->utf8name);
    } else {
        v8::Local<v8::Value> propertyValue = v8impl::V8LocalValueFromJsValue(p->name);

        RETURN_STATUS_IF_FALSE(env, propertyValue->IsName(), JSVM_NAME_EXPECTED);
        *result = propertyValue.As<v8::Name>();
    }

    return JSVM_OK;
}

// convert from jsvm-api property attributes to v8::PropertyAttribute
inline v8::PropertyAttribute V8PropertyAttributesFromDescriptor(const JSVM_PropertyDescriptor* descriptor)
{
    unsigned int attributeFlags = v8::PropertyAttribute::None;

    // The JSVM_WRITABLE attribute is ignored for accessor descriptors, but
    // V8 would throw `TypeError`s on assignment with nonexistence of a setter.
    if ((descriptor->getter == nullptr && descriptor->setter == nullptr) &&
        (descriptor->attributes & JSVM_WRITABLE) == 0) {
        attributeFlags |= v8::PropertyAttribute::ReadOnly;
    }

    if ((descriptor->attributes & JSVM_ENUMERABLE) == 0) {
        attributeFlags |= v8::PropertyAttribute::DontEnum;
    }
    if ((descriptor->attributes & JSVM_CONFIGURABLE) == 0) {
        attributeFlags |= v8::PropertyAttribute::DontDelete;
    }

    return static_cast<v8::PropertyAttribute>(attributeFlags);
}

inline JSVM_Status ConcludeDeferred(JSVM_Env env, JSVM_Deferred deferred, JSVM_Value result, bool isResolved)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, result);

    v8::Local<v8::Context> context = env->context();
    v8impl::Persistent<v8::Value>* deferredRef = PersistentFromJsDeferred(deferred);
    v8::Local<v8::Value> v8Deferred = v8::Local<v8::Value>::New(env->isolate, *deferredRef);

    auto resolver = v8Deferred.As<v8::Promise::Resolver>();

    v8::Maybe<bool> success = isResolved ? resolver->Resolve(context, v8impl::V8LocalValueFromJsValue(result))
                                         : resolver->Reject(context, v8impl::V8LocalValueFromJsValue(result));

    delete deferredRef;
    RETURN_STATUS_IF_FALSE(env, success.FromMaybe(false), JSVM_GENERIC_FAILURE);

    return GET_RETURN_STATUS(env);
}

enum UnwrapAction { KeepWrap, RemoveWrap };

inline JSVM_Status Unwrap(JSVM_Env env, JSVM_Value jsObject, void** result, UnwrapAction action)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, jsObject);
    if (action == KeepWrap) {
        CHECK_ARG(env, result);
    }

    v8::Local<v8::Context> context = env->context();

    v8::Local<v8::Value> value = v8impl::V8LocalValueFromJsValue(jsObject);
    RETURN_STATUS_IF_FALSE(env, value->IsObject(), JSVM_INVALID_ARG);
    v8::Local<v8::Object> obj = value.As<v8::Object>();

    auto val = obj->GetPrivate(context, JSVM_PRIVATE_KEY(env->isolate, wrapper)).ToLocalChecked();
    RETURN_STATUS_IF_FALSE(env, val->IsExternal(), JSVM_INVALID_ARG);
    RuntimeReference* reference = static_cast<v8impl::RuntimeReference*>(val.As<v8::External>()->Value());

    if (result) {
        *result = reference->GetData();
    }

    if (action == RemoveWrap) {
        CHECK(obj->DeletePrivate(context, JSVM_PRIVATE_KEY(env->isolate, wrapper)).FromJust());
        v8impl::RuntimeReference::DeleteReference(reference);
    }

    return GET_RETURN_STATUS(env);
}

//=== Function JSVM_Callback wrapper =================================

// Use this data structure to associate callback data with each N-API function
// exposed to JavaScript. The structure is stored in a v8::External which gets
// passed into our callback wrapper. This reduces the performance impact of
// calling through N-API.
// Ref: benchmark/misc/function_call
// Discussion (incl. perf. data): https://github.com/nodejs/node/pull/21072
class CallbackBundle {
public:
    // Creates an object to be made available to the static function callback
    // wrapper, used to retrieve the native callback function and data pointer.
    static inline v8::Local<v8::Value> New(JSVM_Env env, JSVM_Callback cb)
    {
        return v8::External::New(env->isolate, cb);
    }

    static inline v8::Local<v8::Value> New(JSVM_Env env, v8impl::JSVM_PropertyHandlerCfgStruct* cb)
    {
        return v8::External::New(env->isolate, cb);
    }
};

// Base class extended by classes that wrap V8 function and property callback
// info.
class CallbackWrapper {
public:
    inline CallbackWrapper(JSVM_Value thisArg, size_t argsLength, void* data)
        : receiver(thisArg), argsLength(argsLength), data(data)
    {}

    virtual JSVM_Value GetNewTarget() = 0;

    virtual void GetArgs(JSVM_Value* buffer, size_t bufferLength) = 0;

    virtual void SetReturnValue(JSVM_Value value) = 0;

    JSVM_Value This()
    {
        return receiver;
    }

    size_t ArgsLength()
    {
        return argsLength;
    }

    void* Data()
    {
        return data;
    }

protected:
    const JSVM_Value receiver;
    const size_t argsLength;
    void* data;
};

class CallbackWrapperBase : public CallbackWrapper {
public:
    inline CallbackWrapperBase(const v8::FunctionCallbackInfo<v8::Value>& cbinfo, const size_t argsLength)
        : CallbackWrapper(JsValueFromV8LocalValue(cbinfo.This()), argsLength, nullptr), cbinfo(cbinfo)
    {
        cb = static_cast<JSVM_Callback>(cbinfo.Data().As<v8::External>()->Value());
        data = cb->data;
    }

protected:
    inline const v8::FunctionCallbackInfo<v8::Value>& GetCbInfo()
    {
        return cbinfo;
    }
    inline void InvokeCallback()
    {
        JSVM_CallbackInfo cbinfoWrapper = reinterpret_cast<JSVM_CallbackInfo>(static_cast<CallbackWrapper*>(this));

        // All other pointers we need are stored in `_bundle`
        auto context = cbinfo.GetIsolate()->GetCurrentContext();
        auto env = v8impl::GetContextEnv(context);
        auto func = cb->callback;

        JSVM_Value result = nullptr;
        bool exceptionOccurred = false;
        env->CallIntoModule([&](JSVM_Env env) { result = func(env, cbinfoWrapper); },
                            [&](JSVM_Env env, v8::Local<v8::Value> value) {
                                exceptionOccurred = true;
                                if (env->IsTerminatedOrTerminating()) {
                                    return;
                                }
                                env->isolate->ThrowException(value);
                            });

        if (!exceptionOccurred && (result != nullptr)) {
            this->SetReturnValue(result);
        }
    }

private:
    const v8::FunctionCallbackInfo<v8::Value>& cbinfo;
    JSVM_Callback cb;
};

class FunctionCallbackWrapper : public CallbackWrapperBase {
public:
    static void Invoke(const v8::FunctionCallbackInfo<v8::Value>& info)
    {
        FunctionCallbackWrapper cbwrapper(info);
        cbwrapper.InvokeCallback();
    }

    static inline JSVM_Status NewFunction(JSVM_Env env, JSVM_Callback cb, v8::Local<v8::Function>* result)
    {
        v8::Local<v8::Value> cbdata = v8impl::CallbackBundle::New(env, cb);
        RETURN_STATUS_IF_FALSE(env, !cbdata.IsEmpty(), JSVM_GENERIC_FAILURE);

        v8::MaybeLocal<v8::Function> maybeFunction = v8::Function::New(env->context(), Invoke, cbdata);
        CHECK_MAYBE_EMPTY(env, maybeFunction, JSVM_GENERIC_FAILURE);

        *result = maybeFunction.ToLocalChecked();
        return ClearLastError(env);
    }

    static inline JSVM_Status NewTemplate(JSVM_Env env,
                                          JSVM_Callback cb,
                                          v8::Local<v8::FunctionTemplate>* result,
                                          v8::Local<v8::Signature> sig = v8::Local<v8::Signature>())
    {
        v8::Local<v8::Value> cbdata = v8impl::CallbackBundle::New(env, cb);
        RETURN_STATUS_IF_FALSE(env, !cbdata.IsEmpty(), JSVM_GENERIC_FAILURE);

        *result = v8::FunctionTemplate::New(env->isolate, Invoke, cbdata, sig);
        return ClearLastError(env);
    }

    explicit FunctionCallbackWrapper(const v8::FunctionCallbackInfo<v8::Value>& cbinfo)
        : CallbackWrapperBase(cbinfo, cbinfo.Length())
    {}

    JSVM_Value GetNewTarget() override
    {
        if (GetCbInfo().IsConstructCall()) {
            return v8impl::JsValueFromV8LocalValue(GetCbInfo().NewTarget());
        } else {
            return nullptr;
        }
    }

    /*virtual*/
    void GetArgs(JSVM_Value* buffer, size_t bufferLength) override
    {
        size_t i = 0;
        size_t min = std::min(bufferLength, argsLength);

        const auto& info = GetCbInfo();
        for (; i < min; i += 1) {
            buffer[i] = v8impl::JsValueFromV8LocalValue(info[i]);
        }

        if (i < bufferLength) {
            JSVM_Value undefined = v8impl::JsValueFromV8LocalValue(v8::Undefined(info.GetIsolate()));
            for (; i < bufferLength; i += 1) {
                buffer[i] = undefined;
            }
        }
    }

    /*virtual*/
    void SetReturnValue(JSVM_Value value) override
    {
        v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
        GetCbInfo().GetReturnValue().Set(val);
    }
};

template<typename T>
class PropertyCallbackWrapperBase : public CallbackWrapper {
public:
    inline PropertyCallbackWrapperBase(uint32_t index,
                                       v8::Local<v8::Name> property,
                                       v8::Local<v8::Value> value,
                                       const v8::PropertyCallbackInfo<T>& cbinfo,
                                       const size_t argsLength)
        : CallbackWrapper(JsValueFromV8LocalValue(cbinfo.This()), argsLength, nullptr), cbinfo(cbinfo),
          property(property), value(value), index(index)
    {
        propertyHandler =
            static_cast<v8impl::JSVM_PropertyHandlerCfgStruct*>(cbinfo.Data().template As<v8::External>()->Value());
    }

    JSVM_Value GetNewTarget() override
    {
        return nullptr;
    }

    virtual void GetArgs(JSVM_Value* buffer, size_t bufferlength) override {}

protected:
    inline void NameSetterInvokeCallback()
    {
        auto context = cbinfo.GetIsolate()->GetCurrentContext();
        auto env = v8impl::GetContextEnv(context);
        auto setterCb = propertyHandler->namedSetterCallback_;

        JSVM_Value innerData = nullptr;
        if (propertyHandler->namedPropertyData_ != nullptr) {
            v8impl::UserReference* reference =
                reinterpret_cast<v8impl::UserReference*>(propertyHandler->namedPropertyData_);
            innerData = v8impl::JsValueFromV8LocalValue(reference->Get());
        }

        bool exceptionOccurred = false;
        JSVM_Value result = nullptr;
        JSVM_Value name = JsValueFromV8LocalValue(property);
        JSVM_Value v8Value = JsValueFromV8LocalValue(value);
        JSVM_Value thisArg = this->This();
        env->CallIntoModule(
            [&](JSVM_Env env) {
                if (setterCb) {
                    result = setterCb(env, name, v8Value, thisArg, innerData);
                }
            },
            [&](JSVM_Env env, v8::Local<v8::Value> v8Value) {
                exceptionOccurred = true;
                if (env->IsTerminatedOrTerminating()) {
                    return;
                }
                env->isolate->ThrowException(v8Value);
            });
        if (!exceptionOccurred && (result != nullptr)) {
            this->SetReturnValue(result);
        }
    }

    inline void NameGetterInvokeCallback()
    {
        auto context = cbinfo.GetIsolate()->GetCurrentContext();
        auto env = v8impl::GetContextEnv(context);
        auto getterCb = propertyHandler->namedGetterCallback_;

        JSVM_Value innerData = nullptr;
        if (propertyHandler->namedPropertyData_ != nullptr) {
            v8impl::UserReference* reference =
                reinterpret_cast<v8impl::UserReference*>(propertyHandler->namedPropertyData_);
            innerData = v8impl::JsValueFromV8LocalValue(reference->Get());
        }
        bool exceptionOccurred = false;
        JSVM_Value result = nullptr;
        JSVM_Value name = JsValueFromV8LocalValue(property);
        JSVM_Value thisArg = this->This();
        env->CallIntoModule(
            [&](JSVM_Env env) {
                if (getterCb) {
                    result = getterCb(env, name, thisArg, innerData);
                }
            },
            [&](JSVM_Env env, v8::Local<v8::Value> v8Value) {
                exceptionOccurred = true;
                if (env->IsTerminatedOrTerminating()) {
                    return;
                }
                env->isolate->ThrowException(v8Value);
            });
        if (!exceptionOccurred && (result != nullptr)) {
            this->SetReturnValue(result);
        }
    }

    inline void NameDeleterInvokeCallback()
    {
        auto context = cbinfo.GetIsolate()->GetCurrentContext();
        auto env = v8impl::GetContextEnv(context);
        auto deleterCb = propertyHandler->nameDeleterCallback_;

        JSVM_Value innerData = nullptr;
        if (propertyHandler->namedPropertyData_ != nullptr) {
            v8impl::UserReference* reference =
                reinterpret_cast<v8impl::UserReference*>(propertyHandler->namedPropertyData_);
            innerData = v8impl::JsValueFromV8LocalValue(reference->Get());
        }

        bool exceptionOccurred = false;
        JSVM_Value result = nullptr;
        JSVM_Value name = JsValueFromV8LocalValue(property);
        JSVM_Value thisArg = this->This();
        env->CallIntoModule(
            [&](JSVM_Env env) {
                if (deleterCb) {
                    result = deleterCb(env, name, thisArg, innerData);
                }
            },
            [&](JSVM_Env env, v8::Local<v8::Value> v8Value) {
                exceptionOccurred = true;
                if (env->IsTerminatedOrTerminating()) {
                    return;
                }
                env->isolate->ThrowException(v8Value);
            });
        if (!exceptionOccurred && (result != nullptr)) {
            if (v8impl::V8LocalValueFromJsValue(result)->IsBoolean()) {
                this->SetReturnValue(result);
            }
        }
    }

    inline void NameEnumeratorInvokeCallback()
    {
        auto context = cbinfo.GetIsolate()->GetCurrentContext();
        auto env = v8impl::GetContextEnv(context);
        auto enumeratorCb = propertyHandler->namedEnumeratorCallback_;

        JSVM_Value innerData = nullptr;
        if (propertyHandler->namedPropertyData_ != nullptr) {
            v8impl::UserReference* reference =
                reinterpret_cast<v8impl::UserReference*>(propertyHandler->namedPropertyData_);
            innerData = v8impl::JsValueFromV8LocalValue(reference->Get());
        }

        bool exceptionOccurred = false;
        JSVM_Value result = nullptr;
        JSVM_Value thisArg = this->This();
        env->CallIntoModule(
            [&](JSVM_Env env) {
                if (enumeratorCb) {
                    result = enumeratorCb(env, thisArg, innerData);
                }
            },
            [&](JSVM_Env env, v8::Local<v8::Value> v8Value) {
                exceptionOccurred = true;
                if (env->IsTerminatedOrTerminating()) {
                    return;
                }
                env->isolate->ThrowException(v8Value);
            });
        if (!exceptionOccurred && (result != nullptr)) {
            if (v8impl::V8LocalValueFromJsValue(result)->IsArray()) {
                this->SetReturnValue(result);
            }
        }
    }

    inline void IndexSetterInvokeCallback()
    {
        auto context = cbinfo.GetIsolate()->GetCurrentContext();
        auto env = v8impl::GetContextEnv(context);
        auto indexSetterCb = propertyHandler->indexedSetterCallback_;

        JSVM_Value innerData = nullptr;
        if (propertyHandler->indexedPropertyData_ != nullptr) {
            v8impl::UserReference* reference =
                reinterpret_cast<v8impl::UserReference*>(propertyHandler->indexedPropertyData_);
            innerData = v8impl::JsValueFromV8LocalValue(reference->Get());
        }

        bool exceptionOccurred = false;
        JSVM_Value result = nullptr;
        JSVM_Value v8Index = JsValueFromV8LocalValue(v8::Integer::NewFromUnsigned(env->isolate, index));
        JSVM_Value v8Value = JsValueFromV8LocalValue(value);
        JSVM_Value thisArg = this->This();
        env->CallIntoModule(
            [&](JSVM_Env env) {
                if (indexSetterCb) {
                    result = indexSetterCb(env, v8Index, v8Value, thisArg, innerData);
                }
            },
            [&](JSVM_Env env, v8::Local<v8::Value> v8Value) {
                exceptionOccurred = true;
                if (env->IsTerminatedOrTerminating()) {
                    return;
                }
                env->isolate->ThrowException(v8Value);
            });
        if (!exceptionOccurred && (result != nullptr)) {
            this->SetReturnValue(result);
        }
    }

    inline void IndexGetterInvokeCallback()
    {
        auto context = cbinfo.GetIsolate()->GetCurrentContext();
        auto env = v8impl::GetContextEnv(context);
        auto indexGetterCb = propertyHandler->indexedGetterCallback_;

        JSVM_Value innerData = nullptr;
        if (propertyHandler->indexedPropertyData_ != nullptr) {
            v8impl::UserReference* reference =
                reinterpret_cast<v8impl::UserReference*>(propertyHandler->indexedPropertyData_);
            innerData = v8impl::JsValueFromV8LocalValue(reference->Get());
        }

        JSVM_Value result = nullptr;
        bool exceptionOccurred = false;
        JSVM_Value v8Index = JsValueFromV8LocalValue(v8::Integer::NewFromUnsigned(env->isolate, index));
        JSVM_Value thisArg = this->This();
        env->CallIntoModule(
            [&](JSVM_Env env) {
                if (indexGetterCb) {
                    result = indexGetterCb(env, v8Index, thisArg, innerData);
                }
            },
            [&](JSVM_Env env, v8::Local<v8::Value> v8Value) {
                exceptionOccurred = true;
                if (env->IsTerminatedOrTerminating()) {
                    return;
                }
                env->isolate->ThrowException(v8Value);
            });
        if (!exceptionOccurred && (result != nullptr)) {
            this->SetReturnValue(result);
        }
    }

    inline void IndexDeleterInvokeCallback()
    {
        auto context = cbinfo.GetIsolate()->GetCurrentContext();
        auto env = v8impl::GetContextEnv(context);
        auto indexDeleterCb = propertyHandler->indexedDeleterCallback_;

        JSVM_Value innerData = nullptr;
        if (propertyHandler->indexedPropertyData_ != nullptr) {
            v8impl::UserReference* reference =
                reinterpret_cast<v8impl::UserReference*>(propertyHandler->indexedPropertyData_);
            innerData = v8impl::JsValueFromV8LocalValue(reference->Get());
        }

        bool exceptionOccurred = false;
        JSVM_Value result = nullptr;
        JSVM_Value v8Index = JsValueFromV8LocalValue(v8::Integer::NewFromUnsigned(env->isolate, index));
        JSVM_Value thisArg = this->This();
        env->CallIntoModule(
            [&](JSVM_Env env) {
                if (indexDeleterCb) {
                    result = indexDeleterCb(env, v8Index, thisArg, innerData);
                }
            },
            [&](JSVM_Env env, v8::Local<v8::Value> v8Value) {
                exceptionOccurred = true;
                if (env->IsTerminatedOrTerminating()) {
                    return;
                }
                env->isolate->ThrowException(v8Value);
            });
        if (!exceptionOccurred && (result != nullptr)) {
            if (v8impl::V8LocalValueFromJsValue(result)->IsBoolean()) {
                this->SetReturnValue(result);
            }
        }
    }

    inline void IndexEnumeratorInvokeCallback()
    {
        auto context = cbinfo.GetIsolate()->GetCurrentContext();
        auto env = v8impl::GetContextEnv(context);
        auto enumeratorCb = propertyHandler->indexedEnumeratorCallback_;

        JSVM_Value innerData = nullptr;
        if (propertyHandler->indexedPropertyData_ != nullptr) {
            v8impl::UserReference* reference =
                reinterpret_cast<v8impl::UserReference*>(propertyHandler->indexedPropertyData_);
            innerData = v8impl::JsValueFromV8LocalValue(reference->Get());
        }

        bool exceptionOccurred = false;
        JSVM_Value result = nullptr;
        JSVM_Value thisArg = this->This();
        env->CallIntoModule(
            [&](JSVM_Env env) {
                if (enumeratorCb) {
                    result = enumeratorCb(env, thisArg, innerData);
                }
            },
            [&](JSVM_Env env, v8::Local<v8::Value> v8Value) {
                exceptionOccurred = true;
                if (env->IsTerminatedOrTerminating()) {
                    return;
                }
                env->isolate->ThrowException(v8Value);
            });
        if (!exceptionOccurred && (result != nullptr)) {
            if (v8impl::V8LocalValueFromJsValue(result)->IsArray()) {
                this->SetReturnValue(result);
            }
        }
    }

    const v8::PropertyCallbackInfo<T>& cbinfo;
    JSVM_PropertyHandlerCfgStruct* propertyHandler;
    v8::Local<v8::Name> property;
    v8::Local<v8::Value> value;
    uint32_t index;
};

template<typename T>
class PropertyCallbackWrapper : public PropertyCallbackWrapperBase<T> {
public:
    static void NameSetterInvoke(v8::Local<v8::Name> property,
                                 v8::Local<v8::Value> value,
                                 const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        PropertyCallbackWrapper<v8::Value> propertyCbWrapper(property, value, info);
        propertyCbWrapper.NameSetterInvokeCallback();
    }

    static void NameGetterInvoke(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        PropertyCallbackWrapper<v8::Value> propertyCbWrapper(property, v8::Local<v8::Value>(), info);
        propertyCbWrapper.NameGetterInvokeCallback();
    }

    static void NameDeleterInvoke(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Boolean>& info)
    {
        PropertyCallbackWrapper<v8::Boolean> propertyCbWrapper(property, v8::Local<v8::Value>(), info);
        propertyCbWrapper.NameDeleterInvokeCallback();
    }

    static void NameEnumeratorInvoke(const v8::PropertyCallbackInfo<v8::Array>& info)
    {
        PropertyCallbackWrapper<v8::Array> propertyCbWrapper(v8::Local<v8::Name>(), v8::Local<v8::Value>(), info);
        propertyCbWrapper.NameEnumeratorInvokeCallback();
    }

    static void IndexSetterInvoke(uint32_t index,
                                  v8::Local<v8::Value> value,
                                  const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        PropertyCallbackWrapper<v8::Value> propertyCbWrapper(index, value, info);
        propertyCbWrapper.IndexSetterInvokeCallback();
    }

    static void IndexGetterInvoke(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        PropertyCallbackWrapper<v8::Value> propertyCbWrapper(index, v8::Local<v8::Value>(), info);
        propertyCbWrapper.IndexGetterInvokeCallback();
    }

    static void IndexDeleterInvoke(uint32_t index, const v8::PropertyCallbackInfo<v8::Boolean>& info)
    {
        PropertyCallbackWrapper<v8::Boolean> propertyCbWrapper(index, v8::Local<v8::Value>(), info);
        propertyCbWrapper.IndexDeleterInvokeCallback();
    }

    static void IndexEnumeratorInvoke(const v8::PropertyCallbackInfo<v8::Array>& info)
    {
        PropertyCallbackWrapper<v8::Array> propertyCbWrapper(0, v8::Local<v8::Value>(), info);
        propertyCbWrapper.IndexEnumeratorInvokeCallback();
    }

    explicit PropertyCallbackWrapper(v8::Local<v8::Name> name,
                                     v8::Local<v8::Value> value,
                                     const v8::PropertyCallbackInfo<T>& cbinfo)
        : PropertyCallbackWrapperBase<T>(0, name, value, cbinfo, 0), cbinfo(cbinfo)
    {}

    explicit PropertyCallbackWrapper(uint32_t index,
                                     v8::Local<v8::Value> value,
                                     const v8::PropertyCallbackInfo<T>& cbinfo)
        : PropertyCallbackWrapperBase<T>(index, v8::Local<v8::Name>(), value, cbinfo, 0), cbinfo(cbinfo)
    {}

    /*virtual*/
    void SetReturnValue(JSVM_Value value) override
    {
        v8::Local<T> val = v8impl::V8LocalValueFromJsValue(value).As<T>();
        cbinfo.GetReturnValue().Set(val);
    }

protected:
    const v8::PropertyCallbackInfo<T>& cbinfo;
};

inline JSVM_Status Wrap(JSVM_Env env,
                        JSVM_Value jsObject,
                        void* nativeObject,
                        JSVM_Finalize finalizeCb,
                        void* finalizeHint,
                        JSVM_Ref* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, jsObject);

    v8::Local<v8::Context> context = env->context();

    v8::Local<v8::Value> value = v8impl::V8LocalValueFromJsValue(jsObject);
    RETURN_STATUS_IF_FALSE(env, value->IsObject(), JSVM_INVALID_ARG);
    v8::Local<v8::Object> obj = value.As<v8::Object>();

    // If we've already wrapped this object, we error out.
    RETURN_STATUS_IF_FALSE(env, !obj->HasPrivate(context, JSVM_PRIVATE_KEY(env->isolate, wrapper)).FromJust(),
                           JSVM_INVALID_ARG);

    auto reference = v8impl::RuntimeReference::New(env, obj, finalizeCb, nativeObject, finalizeHint);
    if (result != nullptr) {
        auto* userRef = v8impl::UserReference::New(env, obj, 0);
        *result = reinterpret_cast<JSVM_Ref>(userRef);
    }

    CHECK(obj->SetPrivate(context, JSVM_PRIVATE_KEY(env->isolate, wrapper), v8::External::New(env->isolate, reference))
              .FromJust());

    return GET_RETURN_STATUS(env);
}

} // end of anonymous namespace

} // end of namespace v8impl

v8::Platform* JSVM_Env__::platform()
{
    return v8impl::g_platform.get();
}

JSVM_Status JSVM_CDECL OH_JSVM_Init(const JSVM_InitOptions* options)
{
    static std::atomic<bool> initialized(false);
    if (initialized.load()) {
        return JSVM_GENERIC_FAILURE;
    }
    initialized.store(true);

    OHOS_API_CALL(platform::ohos::WriteHisysevent());
    OHOS_API_CALL(platform::ohos::ReportKeyThread(platform::ohos::ThreadRole::IMPORTANT_DISPLAY));
    v8::V8::InitializePlatform(v8impl::g_platform.get());

    OHOS_API_CALL(platform::ohos::SetSecurityMode());

    if (options && options->argc && options->argv) {
        v8::V8::SetFlagsFromCommandLine(options->argc, options->argv, options->removeFlags);
    }
    v8::V8::Initialize();

    const auto cb = v8impl::FunctionCallbackWrapper::Invoke;
    v8impl::externalReferenceRegistry.push_back((intptr_t)cb);
    if (auto p = options ? options->externalReferences : nullptr) {
        for (; *p != 0; p++) {
            v8impl::externalReferenceRegistry.push_back(*p);
        }
    }
    v8impl::externalReferenceRegistry.push_back(0);
    return JSVM_OK;
}

JSVM_Status JSVM_CDECL OH_JSVM_GetVM(JSVM_Env env, JSVM_VM* result)
{
    *result = reinterpret_cast<JSVM_VM>(env->isolate);
    return JSVM_OK;
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateVM(const JSVM_CreateVMOptions* options, JSVM_VM* result)
{
    OHOS_API_CALL(platform::ohos::ReportKeyThread(platform::ohos::ThreadRole::USER_INTERACT));

    v8::Isolate::CreateParams createParams;
    auto externalReferences = v8impl::externalReferenceRegistry.data();
    createParams.external_references = externalReferences;

    v8::StartupData* snapshotBlob = nullptr;
    if (options && options->snapshotBlobData) {
        snapshotBlob = new v8::StartupData();
        snapshotBlob->data = options->snapshotBlobData;
        snapshotBlob->raw_size = options->snapshotBlobSize;

        if (!snapshotBlob->IsValid()) {
            // TODO: Is VerifyCheckSum necessay if there has been a validity check?
            delete snapshotBlob;
            return JSVM_INVALID_ARG;
        }
        createParams.snapshot_blob = snapshotBlob;
    }

    v8::Isolate* isolate;
    if (options && options->isForSnapshotting) {
        isolate = v8::Isolate::Allocate();
        auto creator = new v8::SnapshotCreator(isolate, externalReferences);
        v8impl::SetIsolateSnapshotCreator(isolate, creator);
    } else {
        createParams.array_buffer_allocator = v8impl::GetOrCreateDefaultArrayBufferAllocator();
        isolate = v8::Isolate::New(createParams);
    }
    v8impl::CreateIsolateData(isolate, snapshotBlob);
    *result = reinterpret_cast<JSVM_VM>(isolate);

    return JSVM_OK;
}

JSVM_Status JSVM_CDECL OH_JSVM_DestroyVM(JSVM_VM vm)
{
    if (!vm) {
        return JSVM_INVALID_ARG;
    }
    auto isolate = reinterpret_cast<v8::Isolate*>(vm);
    auto creator = v8impl::GetIsolateSnapshotCreator(isolate);
    auto data = v8impl::GetIsolateData(isolate);

    if (creator != nullptr) {
        delete creator;
    } else {
        isolate->Dispose();
    }
    if (data != nullptr) {
        delete data;
    }

    return JSVM_OK;
}

JSVM_Status JSVM_CDECL OH_JSVM_OpenVMScope(JSVM_VM vm, JSVM_VMScope* result)
{
    auto isolate = reinterpret_cast<v8::Isolate*>(vm);
    auto scope = new v8::Isolate::Scope(isolate);
    *result = reinterpret_cast<JSVM_VMScope>(scope);
    return JSVM_OK;
}

JSVM_Status JSVM_CDECL OH_JSVM_CloseVMScope(JSVM_VM vm, JSVM_VMScope scope)
{
    auto v8scope = reinterpret_cast<v8::Isolate::Scope*>(scope);
    delete v8scope;
    return JSVM_OK;
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateEnv(JSVM_VM vm,
                                         size_t propertyCount,
                                         const JSVM_PropertyDescriptor* properties,
                                         JSVM_Env* result)
{
    auto isolate = reinterpret_cast<v8::Isolate*>(vm);
    auto env = new JSVM_Env__(isolate, JSVM_API_VERSION);
    v8::HandleScope handleScope(isolate);
    auto globalTemplate = v8::ObjectTemplate::New(isolate);

    for (size_t i = 0; i < propertyCount; i++) {
        const JSVM_PropertyDescriptor* p = properties + i;

        if ((p->attributes & JSVM_STATIC) != 0) {
            // Ignore static properties.
            continue;
        }

        v8::Local<v8::Name> propertyName =
            v8::String::NewFromUtf8(isolate, p->utf8name, v8::NewStringType::kInternalized).ToLocalChecked();

        v8::PropertyAttribute attributes = v8impl::V8PropertyAttributesFromDescriptor(p);

        if (p->getter != nullptr || p->setter != nullptr) {
            v8::Local<v8::FunctionTemplate> getterTpl;
            v8::Local<v8::FunctionTemplate> setterTpl;
            if (p->getter != nullptr) {
                STATUS_CALL(v8impl::FunctionCallbackWrapper::NewTemplate(env, p->getter, &getterTpl));
            }
            if (p->setter != nullptr) {
                STATUS_CALL(v8impl::FunctionCallbackWrapper::NewTemplate(env, p->setter, &setterTpl));
            }

            globalTemplate->SetAccessorProperty(propertyName, getterTpl, setterTpl, attributes);
        } else if (p->method != nullptr) {
            v8::Local<v8::FunctionTemplate> methodTpl;
            STATUS_CALL(v8impl::FunctionCallbackWrapper::NewTemplate(env, p->method, &methodTpl));

            globalTemplate->Set(propertyName, methodTpl, attributes);
        } else {
            v8::Local<v8::Value> value = v8impl::V8LocalValueFromJsValue(p->value);
            globalTemplate->Set(propertyName, value, attributes);
        }
    }

    v8::Local<v8::Context> context = v8::Context::New(isolate, nullptr, globalTemplate);
    env->contextPersistent.Reset(isolate, context);
    v8impl::SetContextEnv(context, env);
    *result = env;
    // The error code is set in constructor function, just return JSVM_OK here.
    return JSVM_OK;
}

JSVM_EXTERN JSVM_Status JSVM_CDECL OH_JSVM_CreateEnvFromSnapshot(JSVM_VM vm, size_t index, JSVM_Env* result)
{
    auto isolate = reinterpret_cast<v8::Isolate*>(vm);
    v8::HandleScope handleScope(isolate);
    auto maybe = v8::Context::FromSnapshot(isolate, index);

    if (maybe.IsEmpty()) {
        *result = nullptr;
        // TODO: return error message.
        return JSVM_GENERIC_FAILURE;
    }

    auto env = new JSVM_Env__(isolate, JSVM_API_VERSION);
    auto context = maybe.ToLocalChecked();
    env->contextPersistent.Reset(isolate, context);
    v8impl::SetContextEnv(context, env);
    *result = env;

    return JSVM_OK;
}

JSVM_Status JSVM_CDECL OH_JSVM_DestroyEnv(JSVM_Env env)
{
    env->DeleteMe();
    return JSVM_OK;
}

JSVM_Status JSVM_CDECL OH_JSVM_OpenEnvScope(JSVM_Env env, JSVM_EnvScope* result)
{
    auto v8scope = new v8::Context::Scope(env->context());
    *result = reinterpret_cast<JSVM_EnvScope>(v8scope);
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CloseEnvScope(JSVM_Env env, JSVM_EnvScope scope)
{
    auto v8scope = reinterpret_cast<v8::Context::Scope*>(scope);
    delete v8scope;
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CompileScript(JSVM_Env env,
                                             JSVM_Value script,
                                             const uint8_t* cachedData,
                                             size_t cachedDataLength,
                                             bool eagerCompile,
                                             bool* cacheRejected,
                                             JSVM_Script* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, script);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> v8Script = v8impl::V8LocalValueFromJsValue(script);

    RETURN_STATUS_IF_FALSE(env, v8Script->IsString(), JSVM_STRING_EXPECTED);

    v8::Local<v8::Context> context = env->context();

    v8::ScriptCompiler::CachedData* cache =
        cachedData ? new v8::ScriptCompiler::CachedData(cachedData, cachedDataLength) : nullptr;
    v8::ScriptCompiler::Source scriptSource(v8Script.As<v8::String>(), cache);
    auto option = cache ? v8::ScriptCompiler::kConsumeCodeCache
                        : (eagerCompile ? v8::ScriptCompiler::kEagerCompile : v8::ScriptCompiler::kNoCompileOptions);

    auto maybeScript = v8::ScriptCompiler::Compile(context, &scriptSource, option);

    if (cache && cacheRejected) {
        *cacheRejected = cache->rejected;
    }

    CHECK_MAYBE_EMPTY(env, maybeScript, JSVM_GENERIC_FAILURE);
    v8::Local<v8::Script> compiledScript = maybeScript.ToLocalChecked();
    *result = reinterpret_cast<JSVM_Script>(env->NewJsvmData(compiledScript));

    return GET_RETURN_STATUS(env);
}

v8::ScriptOrigin CreateScriptOrigin(v8::Isolate* isolate, v8::Local<v8::String> resourceName, v8::ScriptType type)
{
    const int kOptionsLength = 2;
    const uint32_t kOptionsMagicConstant = 0xF1F2F3F0;
    auto options = v8::PrimitiveArray::New(isolate, kOptionsLength);
    options->Set(isolate, 0, v8::Uint32::New(isolate, kOptionsMagicConstant));
    options->Set(isolate, 1, resourceName);
    return v8::ScriptOrigin(isolate, resourceName, 0, 0, false, -1, v8::Local<v8::Value>(), false, false,
                            type == v8::ScriptType::kModule, options);
}

v8::MaybeLocal<v8::Value> PrepareStackTraceCallback(v8::Local<v8::Context> context,
                                                    v8::Local<v8::Value> error,
                                                    v8::Local<v8::Array> trace)
{
    auto* isolate = context->GetIsolate();
    v8::TryCatch tryCatch(isolate);
    v8::Local<v8::String> moduleName = v8::String::NewFromUtf8(isolate, "sourcemap").ToLocalChecked();
    v8::Local<v8::String> moduleSourceString =
        v8::String::NewFromUtf8(isolate, SourceMapRunner.c_str()).ToLocalChecked();

    v8::ScriptOrigin moduleOrigin = CreateScriptOrigin(isolate, moduleName, v8::ScriptType::kClassic);
    v8::Local<v8::Context> moduleContext = v8::Context::New(isolate);
    v8::ScriptCompiler::Source moduleSource(moduleSourceString, moduleOrigin);
    auto script = v8::Script::Compile(moduleContext, moduleSourceString, &moduleOrigin).ToLocalChecked();
    auto result = script->Run(moduleContext).ToLocalChecked();
    auto resultFunc = v8::Local<v8::Function>::Cast(result);

    v8::Local<v8::Value> element = trace->Get(context, 0).ToLocalChecked();
    std::string fileName = "";
    if (element->IsObject()) {
        auto obj = element->ToObject(context);
        auto getFileName = v8::String::NewFromUtf8(isolate, "getFileName", v8::NewStringType::kNormal);
        auto function = obj.ToLocalChecked()->Get(context, getFileName.ToLocalChecked()).ToLocalChecked();
        auto lineNumberFunction = v8::Local<v8::Function>::Cast(function);
        auto fileNameObj = lineNumberFunction->Call(context, obj.ToLocalChecked(), 0, {});
        fileName = std::string(*v8::String::Utf8Value(isolate, fileNameObj.ToLocalChecked()));
    }
    auto&& sourceMapUrl = (!fileName.empty()) ? v8impl::GetSourceMapFromFileName(std::move(fileName)) : "";
    std::ifstream sourceMapfile(sourceMapUrl);
    std::string content = "";
    if (sourceMapfile.good()) {
        std::stringstream buffer;
        buffer << sourceMapfile.rdbuf();
        content = buffer.str();
    }
    auto sourceMapObject =
        v8::String::NewFromUtf8(isolate, content.c_str(), v8::NewStringType::kNormal, content.length());
    v8::Local<v8::Value> args[] = { error, trace, sourceMapObject.ToLocalChecked() };
    return resultFunc->Call(moduleContext, v8::Undefined(isolate), jsvm::ArraySize(args), args);
}

JSVM_Status JSVM_CDECL OH_JSVM_CompileScriptWithOrigin(JSVM_Env env,
                                                       JSVM_Value script,
                                                       const uint8_t* cachedData,
                                                       size_t cachedDataLength,
                                                       bool eagerCompile,
                                                       bool* cacheRejected,
                                                       JSVM_ScriptOrigin* origin,
                                                       JSVM_Script* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, script);
    CHECK_ARG(env, result);
    CHECK_NOT_NULL(origin->resourceName);

    v8::Local<v8::Value> v8Script = v8impl::V8LocalValueFromJsValue(script);

    RETURN_STATUS_IF_FALSE(env, v8Script->IsString(), JSVM_STRING_EXPECTED);

    v8::Local<v8::Context> context = env->context();
    auto* isolate = context->GetIsolate();

    if (origin->sourceMapUrl) {
        v8impl::SetFileToSourceMapMapping(origin->resourceName, origin->sourceMapUrl);
        isolate->SetPrepareStackTraceCallback(PrepareStackTraceCallback);
    }
    auto sourceMapUrl = !origin->sourceMapUrl
                            ? v8::Local<v8::Value>()
                            : v8::String::NewFromUtf8(isolate, origin->sourceMapUrl).ToLocalChecked().As<v8::Value>();
    auto resourceName = v8::String::NewFromUtf8(isolate, origin->resourceName).ToLocalChecked();
    v8::ScriptOrigin scriptOrigin(isolate, resourceName, origin->resourceLineOffset, origin->resourceColumnOffset,
                                  false, -1, sourceMapUrl);

    v8::ScriptCompiler::CachedData* cache =
        cachedData ? new v8::ScriptCompiler::CachedData(cachedData, cachedDataLength) : nullptr;
    v8::ScriptCompiler::Source scriptSource(v8Script.As<v8::String>(), scriptOrigin, cache);
    auto option = cache ? v8::ScriptCompiler::kConsumeCodeCache
                        : (eagerCompile ? v8::ScriptCompiler::kEagerCompile : v8::ScriptCompiler::kNoCompileOptions);

    auto maybeScript = v8::ScriptCompiler::Compile(context, &scriptSource, option);

    if (cache && cacheRejected) {
        *cacheRejected = cache->rejected;
    }
    CHECK_MAYBE_EMPTY(env, maybeScript, JSVM_GENERIC_FAILURE);
    v8::Local<v8::Script> compiledScript = maybeScript.ToLocalChecked();
    *result = reinterpret_cast<JSVM_Script>(env->NewJsvmData(compiledScript));

    return GET_RETURN_STATUS(env);
}

class CompileOptionResolver {
public:
    CompileOptionResolver(size_t length, JSVM_CompileOptions options[], v8::Isolate* isolate)
    {
        for (size_t i = 0; i < length; i++) {
            switch (options[i].id) {
                case JSVM_COMPILE_MODE: {
                    v8Option = static_cast<v8::ScriptCompiler::CompileOptions>(options[i].content.num);
                    break;
                }
                case JSVM_COMPILE_CODE_CACHE: {
                    auto cache = static_cast<JSVM_CodeCache*>(options[i].content.ptr);
                    cachedData =
                        cache->cache ? new v8::ScriptCompiler::CachedData(cache->cache, cache->length) : nullptr;
                    break;
                }
                case JSVM_COMPILE_SCRIPT_ORIGIN: {
                    jsvmOrigin = static_cast<JSVM_ScriptOrigin*>(options[i].content.ptr);
                    break;
                }
                case JSVM_COMPILE_COMPILE_PROFILE: {
                    profile = static_cast<JSVM_CompileProfile*>(options[i].content.ptr);
                    break;
                }
                case JSVM_COMPILE_ENABLE_SOURCE_MAP: {
                    enableSourceMap = options[i].content.boolean;
                    break;
                }
                default: {
                    continue;
                }
            }
        }
        auto sourceString = jsvmOrigin ? jsvmOrigin->resourceName : "script_" + std::to_string(compileCount++);
        auto sourceMapPtr = jsvmOrigin && jsvmOrigin->sourceMapUrl ? jsvmOrigin->sourceMapUrl : nullptr;
        auto sourceMapUrl =
            (jsvmOrigin && jsvmOrigin->sourceMapUrl)
                ? v8::String::NewFromUtf8(isolate, jsvmOrigin->sourceMapUrl).ToLocalChecked().As<v8::Value>()
                : v8::Local<v8::Value>();
        auto resourceName = v8::String::NewFromUtf8(isolate, sourceString.c_str()).ToLocalChecked();
        v8Origin = new v8::ScriptOrigin(isolate, resourceName, jsvmOrigin ? jsvmOrigin->resourceLineOffset : 0,
                                        jsvmOrigin ? jsvmOrigin->resourceColumnOffset : 0, false, -1, sourceMapUrl);
        if (enableSourceMap && sourceMapPtr) {
            v8impl::SetFileToSourceMapMapping(jsvmOrigin->resourceName, sourceMapPtr);
            isolate->SetPrepareStackTraceCallback(PrepareStackTraceCallback);
        }
        if (v8Option == v8::ScriptCompiler::kConsumeCodeCache && !cachedData) {
            hasInvalidOption = true;
        }
    }

    ~CompileOptionResolver()
    {
        delete v8Origin;
        v8Origin = nullptr;
    }

    v8::ScriptCompiler::CompileOptions v8Option = v8::ScriptCompiler::kNoCompileOptions;
    v8::ScriptCompiler::CachedData* cachedData = nullptr;
    v8::ScriptOrigin* v8Origin = nullptr;
    JSVM_CompileProfile* profile = nullptr;
    JSVM_ScriptOrigin* jsvmOrigin = nullptr;
    bool enableSourceMap = false;
    static size_t compileCount;
    bool hasInvalidOption = false;
};

size_t CompileOptionResolver::compileCount = 0;

JSVM_Status JSVM_CDECL OH_JSVM_CompileScriptWithOptions(JSVM_Env env,
                                                        JSVM_Value script,
                                                        size_t optionCount,
                                                        JSVM_CompileOptions options[],
                                                        JSVM_Script* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, script);
    CHECK_ARG(env, result);

    v8::Local<v8::Context> context = env->context();
    auto* isolate = context->GetIsolate();
    CompileOptionResolver optionResolver(optionCount, options, isolate);
    RETURN_STATUS_IF_FALSE(env, !optionResolver.hasInvalidOption, JSVM_INVALID_ARG);

    v8::Local<v8::Value> v8Script = v8impl::V8LocalValueFromJsValue(script);

    RETURN_STATUS_IF_FALSE(env, v8Script->IsString(), JSVM_STRING_EXPECTED);

    v8::ScriptCompiler::Source scriptSource(v8Script.As<v8::String>(), *optionResolver.v8Origin,
                                            optionResolver.cachedData);
    auto maybeScript = v8::ScriptCompiler::Compile(context, &scriptSource, optionResolver.v8Option);
    CHECK_MAYBE_EMPTY(env, maybeScript, JSVM_GENERIC_FAILURE);
    v8::Local<v8::Script> compiledScript = maybeScript.ToLocalChecked();
    *result = reinterpret_cast<JSVM_Script>(env->NewJsvmData(compiledScript));

    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateCodeCache(JSVM_Env env, JSVM_Script script, const uint8_t** data, size_t* length)
{
    CHECK_ENV(env);
    CHECK_ARG(env, script);
    CHECK_ARG(env, data);
    CHECK_ARG(env, length);

    auto jsvmData = reinterpret_cast<JSVM_Data__*>(script);
    auto v8script = jsvmData->ToV8Local<v8::Script>(env->isolate);
    v8::ScriptCompiler::CachedData* cache;
    cache = v8::ScriptCompiler::CreateCodeCache(v8script->GetUnboundScript());

    if (cache == nullptr) {
        return SetLastError(env, JSVM_GENERIC_FAILURE);
    }

    *data = cache->data;
    *length = cache->length;
    cache->buffer_policy = v8::ScriptCompiler::CachedData::BufferNotOwned;
    delete cache;
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_RunScript(JSVM_Env env, JSVM_Script script, JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, script);
    CHECK_ARG(env, result);

    auto jsvmData = reinterpret_cast<JSVM_Data__*>(script);
    auto v8script = jsvmData->ToV8Local<v8::Script>(env->isolate);
    auto scriptResult = v8script->Run(env->context());
    CHECK_MAYBE_EMPTY(env, scriptResult, JSVM_GENERIC_FAILURE);
    *result = v8impl::JsValueFromV8LocalValue(scriptResult.ToLocalChecked());

    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_JsonParse(JSVM_Env env, JSVM_Value json_string, JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, json_string);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(json_string);
    RETURN_STATUS_IF_FALSE(env, val->IsString(), JSVM_STRING_EXPECTED);

    auto maybe = v8::JSON::Parse(env->context(), val.As<v8::String>());
    CHECK_MAYBE_EMPTY(env, maybe, JSVM_GENERIC_FAILURE);
    *result = v8impl::JsValueFromV8LocalValue(maybe.ToLocalChecked());

    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_JsonStringify(JSVM_Env env, JSVM_Value json_object, JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, json_object);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(json_object);

    auto maybe = v8::JSON::Stringify(env->context(), val);
    CHECK_MAYBE_EMPTY(env, maybe, JSVM_GENERIC_FAILURE);
    *result = v8impl::JsValueFromV8LocalValue(maybe.ToLocalChecked());

    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateSnapshot(JSVM_VM vm,
                                              size_t contextCount,
                                              const JSVM_Env* contexts,
                                              const char** blobData,
                                              size_t* blobSize)
{
    auto isolate = reinterpret_cast<v8::Isolate*>(vm);
    auto creator = v8impl::GetIsolateSnapshotCreator(isolate);

    if (creator == nullptr) {
        // TODO: return specific error message.
        return JSVM_GENERIC_FAILURE;
    }
    {
        v8::HandleScope scope(isolate);
        v8::Local<v8::Context> defaultContext = v8::Context::New(isolate);
        creator->SetDefaultContext(defaultContext);
        // NOTE: The order of the added data must be consistent with the order of
        // getting data in v8impl::CreateIsolateData.
        creator->AddData(JSVM_PRIVATE_KEY(isolate, wrapper));
        creator->AddData(JSVM_PRIVATE_KEY(isolate, typeTag));

        for (size_t i = 0; i < contextCount; i++) {
            auto ctx = contexts[i]->context();
            creator->AddData(ctx, ctx);
            creator->AddContext(ctx);
        }
    }
    auto blob = creator->CreateBlob(v8::SnapshotCreator::FunctionCodeHandling::kKeep);
    *blobData = blob.data;
    *blobSize = blob.raw_size;

    return JSVM_OK;
}

JSVM_EXTERN JSVM_Status JSVM_CDECL OH_JSVM_GetVMInfo(JSVM_VMInfo* result)
{
    result->apiVersion = 1;
    result->engine = "v8";
    result->version = V8_VERSION_STRING;
    result->cachedDataVersionTag = v8::ScriptCompiler::CachedDataVersionTag();
    return JSVM_OK;
}

JSVM_EXTERN JSVM_Status JSVM_CDECL OH_JSVM_MemoryPressureNotification(JSVM_Env env, JSVM_MemoryPressureLevel level)
{
    CHECK_ENV(env);
    env->isolate->MemoryPressureNotification(v8::MemoryPressureLevel(level));
    return ClearLastError(env);
}

JSVM_EXTERN JSVM_Status JSVM_CDECL OH_JSVM_GetHeapStatistics(JSVM_VM vm, JSVM_HeapStatistics* result)
{
    auto isolate = reinterpret_cast<v8::Isolate*>(vm);
    v8::HeapStatistics stats;
    isolate->GetHeapStatistics(&stats);
    result->totalHeapSize = stats.total_heap_size();
    result->totalHeapSizeExecutable = stats.total_heap_size_executable();
    result->totalPhysicalSize = stats.total_physical_size();
    result->totalAvailableSize = stats.total_available_size();
    result->usedHeapSize = stats.used_heap_size();
    result->heapSizeLimit = stats.heap_size_limit();
    result->mallocedMemory = stats.malloced_memory();
    result->externalMemory = stats.external_memory();
    result->peakMallocedMemory = stats.peak_malloced_memory();
    result->numberOfNativeContexts = stats.number_of_native_contexts();
    result->numberOfDetachedContexts = stats.number_of_detached_contexts();
    result->totalGlobalHandlesSize = stats.total_global_handles_size();
    result->usedGlobalHandlesSize = stats.used_global_handles_size();
    return JSVM_OK;
}

JSVM_EXTERN JSVM_Status JSVM_CDECL OH_JSVM_StartCpuProfiler(JSVM_VM vm, JSVM_CpuProfiler* result)
{
    auto isolate = reinterpret_cast<v8::Isolate*>(vm);
    auto profiler = v8::CpuProfiler::New(isolate);
    v8::HandleScope scope(isolate);
    v8::CpuProfilingOptions options;
    profiler->Start(v8::String::Empty(isolate), std::move(options));
    *result = reinterpret_cast<JSVM_CpuProfiler>(profiler);
    return JSVM_OK;
}

JSVM_EXTERN JSVM_Status JSVM_CDECL OH_JSVM_StopCpuProfiler(JSVM_VM vm,
                                                           JSVM_CpuProfiler profiler,
                                                           JSVM_OutputStream stream,
                                                           void* streamData)
{
    CHECK_ARG_WITHOUT_ENV(stream);
    auto isolate = reinterpret_cast<v8::Isolate*>(vm);
    auto v8profiler = reinterpret_cast<v8::CpuProfiler*>(profiler);
    v8::HandleScope scope(isolate);
    auto profile = v8profiler->StopProfiling(v8::String::Empty(isolate));
    v8impl::OutputStream os(stream, streamData);
    profile->Serialize(&os);
    return JSVM_OK;
}

JSVM_EXTERN JSVM_Status JSVM_CDECL OH_JSVM_TakeHeapSnapshot(JSVM_VM vm, JSVM_OutputStream stream, void* streamData)
{
    CHECK_ARG_WITHOUT_ENV(stream);
    auto isolate = reinterpret_cast<v8::Isolate*>(vm);
    auto profiler = isolate->GetHeapProfiler();
    auto snapshot = profiler->TakeHeapSnapshot();
    v8impl::OutputStream os(stream, streamData);
    snapshot->Serialize(&os);
    return JSVM_OK;
}

JSVM_EXTERN JSVM_Status JSVM_CDECL OH_JSVM_OpenInspector(JSVM_Env env, const char* host, uint16_t port)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, host);

    std::string inspectorPath;
    std::string hostName(host);
    auto agent = env->GetInspectorAgent();

    if (!agent->Start(inspectorPath, hostName, port)) {
        LOG(Error) << "Open Inspector failed: Please check the internet permisson.";
        return SetLastError(env, JSVM_GENERIC_FAILURE);
    }
    return GET_RETURN_STATUS(env);
}

JSVM_EXTERN JSVM_Status JSVM_CDECL OH_JSVM_CloseInspector(JSVM_Env env)
{
    JSVM_PREAMBLE(env);
    auto agent = env->GetInspectorAgent();
    if (!agent->IsActive()) {
        return JSVM_GENERIC_FAILURE;
    }
    agent->Stop();
    return GET_RETURN_STATUS(env);
}

JSVM_EXTERN JSVM_Status JSVM_CDECL OH_JSVM_WaitForDebugger(JSVM_Env env, bool breakNextLine)
{
    JSVM_PREAMBLE(env);
    auto* agent = env->GetInspectorAgent();
    if (!agent->IsActive()) {
        return JSVM_GENERIC_FAILURE;
    }

    agent->WaitForConnect();
    if (breakNextLine) {
        agent->PauseOnNextJavascriptStatement("Break on debugger attached");
    }

    return GET_RETURN_STATUS(env);
}

JSVM_EXTERN JSVM_Status JSVM_CDECL OH_JSVM_PumpMessageLoop(JSVM_VM vm, bool* result)
{
    auto isolate = reinterpret_cast<v8::Isolate*>(vm);
    *result = v8::platform::PumpMessageLoop(v8impl::g_platform.get(), isolate);
    return JSVM_OK;
}

JSVM_EXTERN JSVM_Status JSVM_CDECL OH_JSVM_PerformMicrotaskCheckpoint(JSVM_VM vm)
{
    auto isolate = reinterpret_cast<v8::Isolate*>(vm);
    isolate->PerformMicrotaskCheckpoint();
    return JSVM_OK;
}

// Warning: Keep in-sync with JSVM_Status enum
static const char* errorMessages[] = {
    nullptr,
    "Invalid argument",
    "An object was expected",
    "A string was expected",
    "A string or symbol was expected",
    "A function was expected",
    "A number was expected",
    "A boolean was expected",
    "An array was expected",
    "Unknown failure",
    "An exception is pending",
    "The async work item was cancelled",
    "OH_JSVM_EscapeHandle already called on scope",
    "Invalid handle scope usage",
    "Invalid callback scope usage",
    "Thread-safe function queue is full",
    "Thread-safe function handle is closing",
    "A bigint was expected",
    "A date was expected",
    "An arraybuffer was expected",
    "A detachable arraybuffer was expected",
    "Main thread would deadlock",
    "External buffers are not allowed",
    "Cannot run JavaScript",
};

JSVM_Status JSVM_CDECL OH_JSVM_GetLastErrorInfo(JSVM_Env env, const JSVM_ExtendedErrorInfo** result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, result);

    // The value of the constant below must be updated to reference the last
    // message in the `JSVM_Status` enum each time a new error message is added.
    // We don't have a jsvm_status_last as this would result in an ABI
    // change each time a message was added.
    const int lastStatus = JSVM_CANNOT_RUN_JS;

    static_assert(jsvm::ArraySize(errorMessages) == lastStatus + 1,
                  "Count of error messages must match count of error values");
    CHECK_LE(env->lastError.errorCode, lastStatus);
    // Wait until someone requests the last error information to fetch the error
    // message string
    env->lastError.errorMessage = errorMessages[env->lastError.errorCode];

    if (env->lastError.errorCode == JSVM_OK) {
        ClearLastError(env);
    }
    *result = &(env->lastError);
    return JSVM_OK;
}

JSVM_Status JSVM_CDECL
OH_JSVM_CreateFunction(JSVM_Env env, const char* utf8name, size_t length, JSVM_Callback cb, JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, result);
    CHECK_ARG(env, cb);

    v8::Local<v8::Function> returnValue;
    v8::EscapableHandleScope scope(env->isolate);
    v8::Local<v8::Function> fn;
    STATUS_CALL(v8impl::FunctionCallbackWrapper::NewFunction(env, cb, &fn));
    returnValue = scope.Escape(fn);

    if (utf8name != nullptr) {
        v8::Local<v8::String> nameString;
        CHECK_NEW_FROM_UTF8_LEN(env, nameString, utf8name, length);
        returnValue->SetName(nameString);
    }

    *result = v8impl::JsValueFromV8LocalValue(returnValue);

    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateFunctionWithScript(JSVM_Env env,
                                                        const char* funcName,
                                                        size_t length,
                                                        size_t argc,
                                                        const JSVM_Value* argv,
                                                        JSVM_Value script,
                                                        JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, script);
    CHECK_ARG(env, result);
    if (argc > 0) {
        CHECK_ARG(env, argv);
        for (auto i = 0; i < argc; i++) {
            RETURN_STATUS_IF_FALSE(env, v8impl::V8LocalValueFromJsValue(argv[i])->IsString(), JSVM_STRING_EXPECTED);
        }
    }

    v8::Local<v8::Value> v8Script = v8impl::V8LocalValueFromJsValue(script);

    RETURN_STATUS_IF_FALSE(env, v8Script->IsString(), JSVM_STRING_EXPECTED);

    v8::ScriptCompiler::Source scriptSource(v8Script.As<v8::String>());

    v8::Local<v8::Context> context = env->context();

    v8::MaybeLocal<v8::Function> maybeFunc =
        v8::ScriptCompiler::CompileFunction(context, &scriptSource, argc,
                                            reinterpret_cast<v8::Local<v8::String>*>(const_cast<JSVM_Value*>(argv)));
    CHECK_MAYBE_EMPTY(env, maybeFunc, JSVM_GENERIC_FAILURE);

    v8::Local<v8::Function> func = maybeFunc.ToLocalChecked();

    if (funcName != nullptr) {
        v8::Local<v8::String> funcNameString;
        CHECK_NEW_FROM_UTF8_LEN(env, funcNameString, funcName, length);
        func->SetName(funcNameString);
    }

    *result = v8impl::JsValueFromV8LocalValue(func);

    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_DefineClass(JSVM_Env env,
                                           const char* utf8name,
                                           size_t length,
                                           JSVM_Callback constructor,
                                           size_t propertyCount,
                                           const JSVM_PropertyDescriptor* properties,
                                           JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, result);
    CHECK_ARG(env, constructor);

    if (propertyCount > 0) {
        CHECK_ARG(env, properties);
    }

    v8::Isolate* isolate = env->isolate;

    v8::EscapableHandleScope scope(isolate);
    v8::Local<v8::FunctionTemplate> tpl;
    STATUS_CALL(v8impl::FunctionCallbackWrapper::NewTemplate(env, constructor, &tpl));

    v8::Local<v8::String> nameString;
    CHECK_NEW_FROM_UTF8_LEN(env, nameString, utf8name, length);
    tpl->SetClassName(nameString);

    size_t staticPropertyCount = 0;
    for (size_t i = 0; i < propertyCount; i++) {
        const JSVM_PropertyDescriptor* p = properties + i;

        if ((p->attributes & JSVM_STATIC) != 0) {
            // Static properties are handled separately below.
            staticPropertyCount++;
            continue;
        }

        v8::Local<v8::Name> propertyName;
        STATUS_CALL(v8impl::V8NameFromPropertyDescriptor(env, p, &propertyName));

        v8::PropertyAttribute attributes = v8impl::V8PropertyAttributesFromDescriptor(p);

        // This code is similar to that in OH_JSVM_DefineProperties(); the
        // difference is it applies to a template instead of an object,
        // and preferred PropertyAttribute for lack of PropertyDescriptor
        // support on ObjectTemplate.
        if (p->getter != nullptr || p->setter != nullptr) {
            v8::Local<v8::FunctionTemplate> getterTpl;
            v8::Local<v8::FunctionTemplate> setterTpl;
            if (p->getter != nullptr) {
                STATUS_CALL(v8impl::FunctionCallbackWrapper::NewTemplate(env, p->getter, &getterTpl));
            }
            if (p->setter != nullptr) {
                STATUS_CALL(v8impl::FunctionCallbackWrapper::NewTemplate(env, p->setter, &setterTpl));
            }

            tpl->PrototypeTemplate()->SetAccessorProperty(propertyName, getterTpl, setterTpl, attributes,
                                                          v8::AccessControl::DEFAULT);
        } else if (p->method != nullptr) {
            v8::Local<v8::FunctionTemplate> t;
            if (p->attributes & JSVM_NO_RECEIVER_CHECK) {
                STATUS_CALL(v8impl::FunctionCallbackWrapper::NewTemplate(env, p->method, &t));
            } else {
                STATUS_CALL(
                    v8impl::FunctionCallbackWrapper::NewTemplate(env, p->method, &t, v8::Signature::New(isolate, tpl)));
            }

            tpl->PrototypeTemplate()->Set(propertyName, t, attributes);
        } else {
            v8::Local<v8::Value> value = v8impl::V8LocalValueFromJsValue(p->value);
            tpl->PrototypeTemplate()->Set(propertyName, value, attributes);
        }
    }

    v8::Local<v8::Context> context = env->context();
    *result = v8impl::JsValueFromV8LocalValue(scope.Escape(tpl->GetFunction(context).ToLocalChecked()));

    if (staticPropertyCount > 0) {
        std::vector<JSVM_PropertyDescriptor> staticDescriptors;
        staticDescriptors.reserve(staticPropertyCount);

        for (size_t i = 0; i < propertyCount; i++) {
            const JSVM_PropertyDescriptor* p = properties + i;
            if ((p->attributes & JSVM_STATIC) != 0) {
                staticDescriptors.push_back(*p);
            }
        }

        STATUS_CALL(OH_JSVM_DefineProperties(env, *result, staticDescriptors.size(), staticDescriptors.data()));
    }

    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetPropertyNames(JSVM_Env env, JSVM_Value object, JSVM_Value* result)
{
    return OH_JSVM_GetAllPropertyNames(env, object, JSVM_KEY_INCLUDE_PROTOTYPES,
                                       static_cast<JSVM_KeyFilter>(JSVM_KEY_ENUMERABLE | JSVM_KEY_SKIP_SYMBOLS),
                                       JSVM_KEY_NUMBERS_TO_STRINGS, result);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetAllPropertyNames(JSVM_Env env,
                                                   JSVM_Value object,
                                                   JSVM_KeyCollectionMode keyMode,
                                                   JSVM_KeyFilter keyFilter,
                                                   JSVM_KeyConversion keyConversion,
                                                   JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, result);

    v8::Local<v8::Context> context = env->context();
    v8::Local<v8::Object> obj;
    CHECK_TO_OBJECT(env, context, obj, object);

    v8::PropertyFilter filter = v8::PropertyFilter::ALL_PROPERTIES;
    if (keyFilter & JSVM_KEY_WRITABLE) {
        filter = static_cast<v8::PropertyFilter>(filter | v8::PropertyFilter::ONLY_WRITABLE);
    }
    if (keyFilter & JSVM_KEY_ENUMERABLE) {
        filter = static_cast<v8::PropertyFilter>(filter | v8::PropertyFilter::ONLY_ENUMERABLE);
    }
    if (keyFilter & JSVM_KEY_CONFIGURABLE) {
        filter = static_cast<v8::PropertyFilter>(filter | v8::PropertyFilter::ONLY_CONFIGURABLE);
    }
    if (keyFilter & JSVM_KEY_SKIP_STRINGS) {
        filter = static_cast<v8::PropertyFilter>(filter | v8::PropertyFilter::SKIP_STRINGS);
    }
    if (keyFilter & JSVM_KEY_SKIP_SYMBOLS) {
        filter = static_cast<v8::PropertyFilter>(filter | v8::PropertyFilter::SKIP_SYMBOLS);
    }
    v8::KeyCollectionMode collectionMode;
    v8::KeyConversionMode conversionMode;

    switch (keyMode) {
        case JSVM_KEY_INCLUDE_PROTOTYPES:
            collectionMode = v8::KeyCollectionMode::kIncludePrototypes;
            break;
        case JSVM_KEY_OWN_ONLY:
            collectionMode = v8::KeyCollectionMode::kOwnOnly;
            break;
        default:
            return SetLastError(env, JSVM_INVALID_ARG);
    }

    switch (keyConversion) {
        case JSVM_KEY_KEEP_NUMBERS:
            conversionMode = v8::KeyConversionMode::kKeepNumbers;
            break;
        case JSVM_KEY_NUMBERS_TO_STRINGS:
            conversionMode = v8::KeyConversionMode::kConvertToString;
            break;
        default:
            return SetLastError(env, JSVM_INVALID_ARG);
    }

    v8::MaybeLocal<v8::Array> maybeAllPropertynames =
        obj->GetPropertyNames(context, collectionMode, filter, v8::IndexFilter::kIncludeIndices, conversionMode);

    CHECK_MAYBE_EMPTY_WITH_PREAMBLE(env, maybeAllPropertynames, JSVM_GENERIC_FAILURE);

    *result = v8impl::JsValueFromV8LocalValue(maybeAllPropertynames.ToLocalChecked());
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_SetProperty(JSVM_Env env, JSVM_Value object, JSVM_Value key, JSVM_Value value)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, key);
    CHECK_ARG(env, value);

    v8::Local<v8::Context> context = env->context();
    v8::Local<v8::Object> obj;

    CHECK_TO_OBJECT(env, context, obj, object);

    v8::Local<v8::Value> k = v8impl::V8LocalValueFromJsValue(key);
    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);

    v8::Maybe<bool> setMaybe = obj->Set(context, k, val);
    RETURN_STATUS_IF_FALSE(env, setMaybe.FromMaybe(false), JSVM_GENERIC_FAILURE);
    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_HasProperty(JSVM_Env env, JSVM_Value object, JSVM_Value key, bool* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, result);
    CHECK_ARG(env, key);

    v8::Local<v8::Context> context = env->context();
    v8::Local<v8::Object> obj;

    CHECK_TO_OBJECT(env, context, obj, object);

    v8::Local<v8::Value> k = v8impl::V8LocalValueFromJsValue(key);
    v8::Maybe<bool> hasMaybe = obj->Has(context, k);
    CHECK_MAYBE_NOTHING(env, hasMaybe, JSVM_GENERIC_FAILURE);

    *result = hasMaybe.FromMaybe(false);
    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetProperty(JSVM_Env env, JSVM_Value object, JSVM_Value key, JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, key);
    CHECK_ARG(env, result);

    v8::Local<v8::Context> context = env->context();
    v8::Local<v8::Value> k = v8impl::V8LocalValueFromJsValue(key);
    v8::Local<v8::Object> obj;

    CHECK_TO_OBJECT(env, context, obj, object);

    auto getMaybe = obj->Get(context, k);
    CHECK_MAYBE_EMPTY(env, getMaybe, JSVM_GENERIC_FAILURE);

    v8::Local<v8::Value> val = getMaybe.ToLocalChecked();
    *result = v8impl::JsValueFromV8LocalValue(val);
    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_DeleteProperty(JSVM_Env env, JSVM_Value object, JSVM_Value key, bool* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, key);

    v8::Local<v8::Context> context = env->context();
    v8::Local<v8::Value> k = v8impl::V8LocalValueFromJsValue(key);
    v8::Local<v8::Object> obj;

    CHECK_TO_OBJECT(env, context, obj, object);
    v8::Maybe<bool> deleteMaybe = obj->Delete(context, k);
    CHECK_MAYBE_NOTHING(env, deleteMaybe, JSVM_GENERIC_FAILURE);

    if (result != nullptr)
        *result = deleteMaybe.FromMaybe(false);

    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_HasOwnProperty(JSVM_Env env, JSVM_Value object, JSVM_Value key, bool* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, key);
    CHECK_ARG(env, result);

    v8::Local<v8::Context> context = env->context();
    v8::Local<v8::Object> obj;

    CHECK_TO_OBJECT(env, context, obj, object);
    v8::Local<v8::Value> k = v8impl::V8LocalValueFromJsValue(key);
    RETURN_STATUS_IF_FALSE(env, k->IsName(), JSVM_NAME_EXPECTED);
    v8::Maybe<bool> hasMaybe = obj->HasOwnProperty(context, k.As<v8::Name>());
    CHECK_MAYBE_NOTHING(env, hasMaybe, JSVM_GENERIC_FAILURE);
    *result = hasMaybe.FromMaybe(false);

    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_SetNamedProperty(JSVM_Env env, JSVM_Value object, const char* utf8name, JSVM_Value value)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, value);

    v8::Local<v8::Context> context = env->context();
    v8::Local<v8::Object> obj;

    CHECK_TO_OBJECT(env, context, obj, object);

    v8::Local<v8::Name> key;
    CHECK_NEW_FROM_UTF8(env, key, utf8name);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);

    v8::Maybe<bool> setMaybe = obj->Set(context, key, val);
    RETURN_STATUS_IF_FALSE(env, setMaybe.FromMaybe(false), JSVM_GENERIC_FAILURE);
    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_HasNamedProperty(JSVM_Env env, JSVM_Value object, const char* utf8name, bool* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, result);

    v8::Local<v8::Context> context = env->context();
    v8::Local<v8::Object> obj;

    CHECK_TO_OBJECT(env, context, obj, object);

    v8::Local<v8::Name> key;
    CHECK_NEW_FROM_UTF8(env, key, utf8name);

    v8::Maybe<bool> hasMaybe = obj->Has(context, key);
    CHECK_MAYBE_NOTHING(env, hasMaybe, JSVM_GENERIC_FAILURE);

    *result = hasMaybe.FromMaybe(false);
    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetNamedProperty(JSVM_Env env,
                                                JSVM_Value object,
                                                const char* utf8name,
                                                JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, result);

    v8::Local<v8::Context> context = env->context();

    v8::Local<v8::Name> key;
    CHECK_NEW_FROM_UTF8(env, key, utf8name);

    v8::Local<v8::Object> obj;

    CHECK_TO_OBJECT(env, context, obj, object);

    auto getMaybe = obj->Get(context, key);
    CHECK_MAYBE_EMPTY(env, getMaybe, JSVM_GENERIC_FAILURE);

    v8::Local<v8::Value> val = getMaybe.ToLocalChecked();
    *result = v8impl::JsValueFromV8LocalValue(val);
    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_SetElement(JSVM_Env env, JSVM_Value object, uint32_t index, JSVM_Value value)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, value);

    v8::Local<v8::Context> context = env->context();
    v8::Local<v8::Object> obj;

    CHECK_TO_OBJECT(env, context, obj, object);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    auto setMaybe = obj->Set(context, index, val);
    RETURN_STATUS_IF_FALSE(env, setMaybe.FromMaybe(false), JSVM_GENERIC_FAILURE);

    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_HasElement(JSVM_Env env, JSVM_Value object, uint32_t index, bool* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, result);

    v8::Local<v8::Context> context = env->context();
    v8::Local<v8::Object> obj;

    CHECK_TO_OBJECT(env, context, obj, object);

    v8::Maybe<bool> hasMaybe = obj->Has(context, index);
    CHECK_MAYBE_NOTHING(env, hasMaybe, JSVM_GENERIC_FAILURE);

    *result = hasMaybe.FromMaybe(false);
    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetElement(JSVM_Env env, JSVM_Value object, uint32_t index, JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, result);

    v8::Local<v8::Context> context = env->context();
    v8::Local<v8::Object> obj;

    CHECK_TO_OBJECT(env, context, obj, object);

    auto getMaybe = obj->Get(context, index);
    CHECK_MAYBE_EMPTY(env, getMaybe, JSVM_GENERIC_FAILURE);

    *result = v8impl::JsValueFromV8LocalValue(getMaybe.ToLocalChecked());
    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_DeleteElement(JSVM_Env env, JSVM_Value object, uint32_t index, bool* result)
{
    JSVM_PREAMBLE(env);

    v8::Local<v8::Context> context = env->context();
    v8::Local<v8::Object> obj;

    CHECK_TO_OBJECT(env, context, obj, object);
    v8::Maybe<bool> deleteMaybe = obj->Delete(context, index);
    CHECK_MAYBE_NOTHING(env, deleteMaybe, JSVM_GENERIC_FAILURE);

    if (result != nullptr)
        *result = deleteMaybe.FromMaybe(false);

    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_DefineProperties(JSVM_Env env,
                                                JSVM_Value object,
                                                size_t propertyCount,
                                                const JSVM_PropertyDescriptor* properties)
{
    JSVM_PREAMBLE(env);
    if (propertyCount > 0) {
        CHECK_ARG(env, properties);
    }

    v8::Local<v8::Context> context = env->context();

    v8::Local<v8::Object> obj;
    CHECK_TO_OBJECT(env, context, obj, object);

    for (size_t i = 0; i < propertyCount; i++) {
        const JSVM_PropertyDescriptor* p = &properties[i];

        v8::Local<v8::Name> propertyName;
        STATUS_CALL(v8impl::V8NameFromPropertyDescriptor(env, p, &propertyName));

        if (p->getter != nullptr || p->setter != nullptr) {
            v8::Local<v8::Function> localGetter;
            v8::Local<v8::Function> localSetter;

            if (p->getter != nullptr) {
                STATUS_CALL(v8impl::FunctionCallbackWrapper::NewFunction(env, p->getter, &localGetter));
            }
            if (p->setter != nullptr) {
                STATUS_CALL(v8impl::FunctionCallbackWrapper::NewFunction(env, p->setter, &localSetter));
            }

            v8::PropertyDescriptor descriptor(localGetter, localSetter);
            descriptor.set_enumerable((p->attributes & JSVM_ENUMERABLE) != 0);
            descriptor.set_configurable((p->attributes & JSVM_CONFIGURABLE) != 0);

            auto defineMaybe = obj->DefineProperty(context, propertyName, descriptor);

            if (!defineMaybe.FromMaybe(false)) {
                return SetLastError(env, JSVM_INVALID_ARG);
            }
        } else if (p->method != nullptr) {
            v8::Local<v8::Function> method;
            STATUS_CALL(v8impl::FunctionCallbackWrapper::NewFunction(env, p->method, &method));
            v8::PropertyDescriptor descriptor(method, (p->attributes & JSVM_WRITABLE) != 0);
            descriptor.set_enumerable((p->attributes & JSVM_ENUMERABLE) != 0);
            descriptor.set_configurable((p->attributes & JSVM_CONFIGURABLE) != 0);

            auto defineMaybe = obj->DefineProperty(context, propertyName, descriptor);

            if (!defineMaybe.FromMaybe(false)) {
                return SetLastError(env, JSVM_GENERIC_FAILURE);
            }
        } else {
            v8::Local<v8::Value> value = v8impl::V8LocalValueFromJsValue(p->value);
            bool definedSuccessfully = false;

            if ((p->attributes & JSVM_ENUMERABLE) && (p->attributes & JSVM_WRITABLE) &&
                (p->attributes & JSVM_CONFIGURABLE)) {
                // Use a fast path for this type of data property.
                auto defineMaybe = obj->CreateDataProperty(context, propertyName, value);
                definedSuccessfully = defineMaybe.FromMaybe(false);
            } else {
                v8::PropertyDescriptor descriptor(value, (p->attributes & JSVM_WRITABLE) != 0);
                descriptor.set_enumerable((p->attributes & JSVM_ENUMERABLE) != 0);
                descriptor.set_configurable((p->attributes & JSVM_CONFIGURABLE) != 0);

                auto defineMaybe = obj->DefineProperty(context, propertyName, descriptor);
                definedSuccessfully = defineMaybe.FromMaybe(false);
            }

            if (!definedSuccessfully) {
                return SetLastError(env, JSVM_INVALID_ARG);
            }
        }
    }

    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_ObjectFreeze(JSVM_Env env, JSVM_Value object)
{
    JSVM_PREAMBLE(env);

    v8::Local<v8::Context> context = env->context();
    v8::Local<v8::Object> obj;

    CHECK_TO_OBJECT(env, context, obj, object);

    v8::Maybe<bool> setFrozen = obj->SetIntegrityLevel(context, v8::IntegrityLevel::kFrozen);

    RETURN_STATUS_IF_FALSE_WITH_PREAMBLE(env, setFrozen.FromMaybe(false), JSVM_GENERIC_FAILURE);

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_ObjectSeal(JSVM_Env env, JSVM_Value object)
{
    JSVM_PREAMBLE(env);

    v8::Local<v8::Context> context = env->context();
    v8::Local<v8::Object> obj;

    CHECK_TO_OBJECT(env, context, obj, object);

    v8::Maybe<bool> setSealed = obj->SetIntegrityLevel(context, v8::IntegrityLevel::kSealed);

    RETURN_STATUS_IF_FALSE_WITH_PREAMBLE(env, setSealed.FromMaybe(false), JSVM_GENERIC_FAILURE);

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsArray(JSVM_Env env, JSVM_Value value, bool* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);

    *result = val->IsArray();
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsRegExp(JSVM_Env env, JSVM_Value value, bool* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);

    *result = val->IsRegExp();
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetArrayLength(JSVM_Env env, JSVM_Value value, uint32_t* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    RETURN_STATUS_IF_FALSE(env, val->IsArray(), JSVM_ARRAY_EXPECTED);

    v8::Local<v8::Array> arr = val.As<v8::Array>();
    *result = arr->Length();

    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_StrictEquals(JSVM_Env env, JSVM_Value lhs, JSVM_Value rhs, bool* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, lhs);
    CHECK_ARG(env, rhs);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> a = v8impl::V8LocalValueFromJsValue(lhs);
    v8::Local<v8::Value> b = v8impl::V8LocalValueFromJsValue(rhs);

    *result = a->StrictEquals(b);
    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_Equals(JSVM_Env env, JSVM_Value lhs, JSVM_Value rhs, bool* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, lhs);
    CHECK_ARG(env, rhs);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> a = v8impl::V8LocalValueFromJsValue(lhs);
    v8::Local<v8::Value> b = v8impl::V8LocalValueFromJsValue(rhs);
    v8::Local<v8::Context> context = env->context();

    *result = a->Equals(context, b).FromJust();
    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetPrototype(JSVM_Env env, JSVM_Value object, JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, result);

    v8::Local<v8::Context> context = env->context();

    v8::Local<v8::Object> obj;
    CHECK_TO_OBJECT(env, context, obj, object);

    v8::Local<v8::Value> val = obj->GetPrototype();
    *result = v8impl::JsValueFromV8LocalValue(val);
    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateObject(JSVM_Env env, JSVM_Value* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, result);

    *result = v8impl::JsValueFromV8LocalValue(v8::Object::New(env->isolate));

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateArray(JSVM_Env env, JSVM_Value* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, result);

    *result = v8impl::JsValueFromV8LocalValue(v8::Array::New(env->isolate));

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateArrayWithLength(JSVM_Env env, size_t length, JSVM_Value* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, result);

    *result = v8impl::JsValueFromV8LocalValue(v8::Array::New(env->isolate, length));

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateStringLatin1(JSVM_Env env, const char* str, size_t length, JSVM_Value* result)
{
    return v8impl::NewString(env, str, length, result, [&](v8::Isolate* isolate) {
        return v8::String::NewFromOneByte(isolate, reinterpret_cast<const uint8_t*>(str), v8::NewStringType::kNormal,
                                          length);
    });
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateStringUtf8(JSVM_Env env, const char* str, size_t length, JSVM_Value* result)
{
    return v8impl::NewString(env, str, length, result, [&](v8::Isolate* isolate) {
        return v8::String::NewFromUtf8(isolate, str, v8::NewStringType::kNormal, static_cast<int>(length));
    });
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateStringUtf16(JSVM_Env env, const char16_t* str, size_t length, JSVM_Value* result)
{
    return v8impl::NewString(env, str, length, result, [&](v8::Isolate* isolate) {
        return v8::String::NewFromTwoByte(isolate, reinterpret_cast<const uint16_t*>(str), v8::NewStringType::kNormal,
                                          length);
    });
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateDouble(JSVM_Env env, double value, JSVM_Value* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, result);

    *result = v8impl::JsValueFromV8LocalValue(v8::Number::New(env->isolate, value));

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateInt32(JSVM_Env env, int32_t value, JSVM_Value* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, result);

    *result = v8impl::JsValueFromV8LocalValue(v8::Integer::New(env->isolate, value));

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateUint32(JSVM_Env env, uint32_t value, JSVM_Value* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, result);

    *result = v8impl::JsValueFromV8LocalValue(v8::Integer::NewFromUnsigned(env->isolate, value));

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateInt64(JSVM_Env env, int64_t value, JSVM_Value* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, result);

    *result = v8impl::JsValueFromV8LocalValue(v8::Number::New(env->isolate, static_cast<double>(value)));

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateBigintInt64(JSVM_Env env, int64_t value, JSVM_Value* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, result);

    *result = v8impl::JsValueFromV8LocalValue(v8::BigInt::New(env->isolate, value));

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateBigintUint64(JSVM_Env env, uint64_t value, JSVM_Value* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, result);

    *result = v8impl::JsValueFromV8LocalValue(v8::BigInt::NewFromUnsigned(env->isolate, value));

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL
OH_JSVM_CreateBigintWords(JSVM_Env env, int signBit, size_t wordCount, const uint64_t* words, JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, words);
    CHECK_ARG(env, result);

    v8::Local<v8::Context> context = env->context();

    RETURN_STATUS_IF_FALSE(env, wordCount <= INT_MAX, JSVM_INVALID_ARG);

    v8::MaybeLocal<v8::BigInt> b = v8::BigInt::NewFromWords(context, signBit, wordCount, words);

    CHECK_MAYBE_EMPTY_WITH_PREAMBLE(env, b, JSVM_GENERIC_FAILURE);

    *result = v8impl::JsValueFromV8LocalValue(b.ToLocalChecked());
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetBoolean(JSVM_Env env, bool value, JSVM_Value* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, result);

    v8::Isolate* isolate = env->isolate;

    if (value) {
        *result = v8impl::JsValueFromV8LocalValue(v8::True(isolate));
    } else {
        *result = v8impl::JsValueFromV8LocalValue(v8::False(isolate));
    }

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateSymbol(JSVM_Env env, JSVM_Value description, JSVM_Value* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, result);

    v8::Isolate* isolate = env->isolate;

    if (description == nullptr) {
        *result = v8impl::JsValueFromV8LocalValue(v8::Symbol::New(isolate));
    } else {
        v8::Local<v8::Value> desc = v8impl::V8LocalValueFromJsValue(description);
        RETURN_STATUS_IF_FALSE(env, desc->IsString(), JSVM_STRING_EXPECTED);

        *result = v8impl::JsValueFromV8LocalValue(v8::Symbol::New(isolate, desc.As<v8::String>()));
    }

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_SymbolFor(JSVM_Env env, const char* utf8description, size_t length, JSVM_Value* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, result);

    JSVM_Value jsDescriptionString;
    STATUS_CALL(OH_JSVM_CreateStringUtf8(env, utf8description, length, &jsDescriptionString));
    v8::Local<v8::String> descriptionString = v8impl::V8LocalValueFromJsValue(jsDescriptionString).As<v8::String>();

    *result = v8impl::JsValueFromV8LocalValue(v8::Symbol::For(env->isolate, descriptionString));

    return ClearLastError(env);
}

static inline JSVM_Status SetErrorCode(JSVM_Env env,
                                       v8::Local<v8::Value> error,
                                       JSVM_Value code,
                                       const char* codeCstring)
{
    if ((code != nullptr) || (codeCstring != nullptr)) {
        v8::Local<v8::Context> context = env->context();
        v8::Local<v8::Object> errObject = error.As<v8::Object>();

        v8::Local<v8::Value> codeValue = v8impl::V8LocalValueFromJsValue(code);
        if (code != nullptr) {
            codeValue = v8impl::V8LocalValueFromJsValue(code);
            RETURN_STATUS_IF_FALSE(env, codeValue->IsString(), JSVM_STRING_EXPECTED);
        } else {
            CHECK_NEW_FROM_UTF8(env, codeValue, codeCstring);
        }

        v8::Local<v8::Name> codeKey;
        CHECK_NEW_FROM_UTF8(env, codeKey, "code");

        v8::Maybe<bool> setMaybe = errObject->Set(context, codeKey, codeValue);
        RETURN_STATUS_IF_FALSE(env, setMaybe.FromMaybe(false), JSVM_GENERIC_FAILURE);
    }
    return JSVM_OK;
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateError(JSVM_Env env, JSVM_Value code, JSVM_Value msg, JSVM_Value* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, msg);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> messageValue = v8impl::V8LocalValueFromJsValue(msg);
    RETURN_STATUS_IF_FALSE(env, messageValue->IsString(), JSVM_STRING_EXPECTED);

    v8::Local<v8::Value> errorObj = v8::Exception::Error(messageValue.As<v8::String>());
    STATUS_CALL(SetErrorCode(env, errorObj, code, nullptr));

    *result = v8impl::JsValueFromV8LocalValue(errorObj);

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateTypeError(JSVM_Env env, JSVM_Value code, JSVM_Value msg, JSVM_Value* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, msg);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> messageValue = v8impl::V8LocalValueFromJsValue(msg);
    RETURN_STATUS_IF_FALSE(env, messageValue->IsString(), JSVM_STRING_EXPECTED);

    v8::Local<v8::Value> errorObj = v8::Exception::TypeError(messageValue.As<v8::String>());
    STATUS_CALL(SetErrorCode(env, errorObj, code, nullptr));

    *result = v8impl::JsValueFromV8LocalValue(errorObj);

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateRangeError(JSVM_Env env, JSVM_Value code, JSVM_Value msg, JSVM_Value* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, msg);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> messageValue = v8impl::V8LocalValueFromJsValue(msg);
    RETURN_STATUS_IF_FALSE(env, messageValue->IsString(), JSVM_STRING_EXPECTED);

    v8::Local<v8::Value> errorObj = v8::Exception::RangeError(messageValue.As<v8::String>());
    STATUS_CALL(SetErrorCode(env, errorObj, code, nullptr));

    *result = v8impl::JsValueFromV8LocalValue(errorObj);

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateSyntaxError(JSVM_Env env, JSVM_Value code, JSVM_Value msg, JSVM_Value* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, msg);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> messageValue = v8impl::V8LocalValueFromJsValue(msg);
    RETURN_STATUS_IF_FALSE(env, messageValue->IsString(), JSVM_STRING_EXPECTED);

    v8::Local<v8::Value> errorObj = v8::Exception::SyntaxError(messageValue.As<v8::String>());
    STATUS_CALL(SetErrorCode(env, errorObj, code, nullptr));

    *result = v8impl::JsValueFromV8LocalValue(errorObj);

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_Typeof(JSVM_Env env, JSVM_Value value, JSVM_ValueType* result)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8 calls here cannot throw
    // JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> v = v8impl::V8LocalValueFromJsValue(value);

    if (v->IsNumber()) {
        *result = JSVM_NUMBER;
    } else if (v->IsBigInt()) {
        *result = JSVM_BIGINT;
    } else if (v->IsString()) {
        *result = JSVM_STRING;
    } else if (v->IsFunction()) {
        // This test has to come before IsObject because IsFunction
        // implies IsObject
        *result = JSVM_FUNCTION;
    } else if (v->IsExternal()) {
        // This test has to come before IsObject because IsExternal
        // implies IsObject
        *result = JSVM_EXTERNAL;
    } else if (v->IsObject()) {
        *result = JSVM_OBJECT;
    } else if (v->IsBoolean()) {
        *result = JSVM_BOOLEAN;
    } else if (v->IsUndefined()) {
        *result = JSVM_UNDEFINED;
    } else if (v->IsSymbol()) {
        *result = JSVM_SYMBOL;
    } else if (v->IsNull()) {
        *result = JSVM_NULL;
    } else {
        // Should not get here unless V8 has added some new kind of value.
        return SetLastError(env, JSVM_INVALID_ARG);
    }

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetUndefined(JSVM_Env env, JSVM_Value* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, result);

    *result = v8impl::JsValueFromV8LocalValue(v8::Undefined(env->isolate));

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetNull(JSVM_Env env, JSVM_Value* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, result);

    *result = v8impl::JsValueFromV8LocalValue(v8::Null(env->isolate));

    return ClearLastError(env);
}

// Gets all callback info in a single call. (Ugly, but faster.)
JSVM_Status JSVM_CDECL OH_JSVM_GetCbInfo(JSVM_Env env,             // [in] JSVM environment handle
                                         JSVM_CallbackInfo cbinfo, // [in] Opaque callback-info handle
                                         size_t* argc,        // [in-out] Specifies the size of the provided argv array
                                                              // and receives the actual count of args.
                                         JSVM_Value* argv,    // [out] Array of values
                                         JSVM_Value* thisArg, // [out] Receives the JS 'this' arg for the call
                                         void** data)
{ // [out] Receives the data pointer for the callback.
    CHECK_ENV(env);
    CHECK_ARG(env, cbinfo);

    v8impl::CallbackWrapper* info = reinterpret_cast<v8impl::CallbackWrapper*>(cbinfo);

    if (argv != nullptr) {
        CHECK_ARG(env, argc);
        info->GetArgs(argv, *argc);
    }
    if (argc != nullptr) {
        *argc = info->ArgsLength();
    }
    if (thisArg != nullptr) {
        *thisArg = info->This();
    }
    if (data != nullptr) {
        *data = info->Data();
    }

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetNewTarget(JSVM_Env env, JSVM_CallbackInfo cbinfo, JSVM_Value* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, cbinfo);
    CHECK_ARG(env, result);

    v8impl::CallbackWrapper* info = reinterpret_cast<v8impl::CallbackWrapper*>(cbinfo);

    *result = info->GetNewTarget();
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CallFunction(JSVM_Env env,
                                            JSVM_Value recv,
                                            JSVM_Value func,
                                            size_t argc,
                                            const JSVM_Value* argv,
                                            JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, recv);
    if (argc > 0) {
        CHECK_ARG(env, argv);
    }

    v8::Local<v8::Context> context = env->context();

    v8::Local<v8::Value> v8recv = v8impl::V8LocalValueFromJsValue(recv);

    v8::Local<v8::Function> v8func;
    CHECK_TO_FUNCTION(env, v8func, func);

    auto maybe =
        v8func->Call(context, v8recv, argc, reinterpret_cast<v8::Local<v8::Value>*>(const_cast<JSVM_Value*>(argv)));

    RETURN_IF_EXCEPTION_HAS_CAUGHT(env);

    if (result != nullptr) {
        CHECK_MAYBE_EMPTY(env, maybe, JSVM_GENERIC_FAILURE);
        *result = v8impl::JsValueFromV8LocalValue(maybe.ToLocalChecked());
    }
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetGlobal(JSVM_Env env, JSVM_Value* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, result);

    *result = v8impl::JsValueFromV8LocalValue(env->context()->Global());

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_Throw(JSVM_Env env, JSVM_Value error)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, error);

    v8::Isolate* isolate = env->isolate;

    isolate->ThrowException(v8impl::V8LocalValueFromJsValue(error));
    // any VM calls after this point and before returning
    // to the javascript invoker will fail
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_ThrowError(JSVM_Env env, const char* code, const char* msg)
{
    JSVM_PREAMBLE(env);

    v8::Isolate* isolate = env->isolate;
    v8::Local<v8::String> str;
    CHECK_NEW_FROM_UTF8(env, str, msg);

    v8::Local<v8::Value> errorObj = v8::Exception::Error(str);
    STATUS_CALL(SetErrorCode(env, errorObj, nullptr, code));

    isolate->ThrowException(errorObj);
    // any VM calls after this point and before returning
    // to the javascript invoker will fail
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_ThrowTypeError(JSVM_Env env, const char* code, const char* msg)
{
    JSVM_PREAMBLE(env);

    v8::Isolate* isolate = env->isolate;
    v8::Local<v8::String> str;
    CHECK_NEW_FROM_UTF8(env, str, msg);

    v8::Local<v8::Value> errorObj = v8::Exception::TypeError(str);
    STATUS_CALL(SetErrorCode(env, errorObj, nullptr, code));

    isolate->ThrowException(errorObj);
    // any VM calls after this point and before returning
    // to the javascript invoker will fail
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_ThrowRangeError(JSVM_Env env, const char* code, const char* msg)
{
    JSVM_PREAMBLE(env);

    v8::Isolate* isolate = env->isolate;
    v8::Local<v8::String> str;
    CHECK_NEW_FROM_UTF8(env, str, msg);

    v8::Local<v8::Value> errorObj = v8::Exception::RangeError(str);
    STATUS_CALL(SetErrorCode(env, errorObj, nullptr, code));

    isolate->ThrowException(errorObj);
    // any VM calls after this point and before returning
    // to the javascript invoker will fail
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_ThrowSyntaxError(JSVM_Env env, const char* code, const char* msg)
{
    JSVM_PREAMBLE(env);

    v8::Isolate* isolate = env->isolate;
    v8::Local<v8::String> str;
    CHECK_NEW_FROM_UTF8(env, str, msg);

    v8::Local<v8::Value> errorObj = v8::Exception::SyntaxError(str);
    STATUS_CALL(SetErrorCode(env, errorObj, nullptr, code));

    isolate->ThrowException(errorObj);
    // any VM calls after this point and before returning
    // to the javascript invoker will fail
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsError(JSVM_Env env, JSVM_Value value, bool* result)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8 calls here cannot
    // throw JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    *result = val->IsNativeError();

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetValueDouble(JSVM_Env env, JSVM_Value value, double* result)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8 calls here cannot throw
    // JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    RETURN_STATUS_IF_FALSE(env, val->IsNumber(), JSVM_NUMBER_EXPECTED);

    *result = val.As<v8::Number>()->Value();

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetValueInt32(JSVM_Env env, JSVM_Value value, int32_t* result)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8 calls here cannot throw
    // JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);

    if (val->IsInt32()) {
        *result = val.As<v8::Int32>()->Value();
    } else {
        RETURN_STATUS_IF_FALSE(env, val->IsNumber(), JSVM_NUMBER_EXPECTED);

        // Empty context: https://github.com/nodejs/node/issues/14379
        v8::Local<v8::Context> context;
        *result = val->Int32Value(context).FromJust();
    }

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetValueUint32(JSVM_Env env, JSVM_Value value, uint32_t* result)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8 calls here cannot throw
    // JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);

    if (val->IsUint32()) {
        *result = val.As<v8::Uint32>()->Value();
    } else {
        RETURN_STATUS_IF_FALSE(env, val->IsNumber(), JSVM_NUMBER_EXPECTED);

        // Empty context: https://github.com/nodejs/node/issues/14379
        v8::Local<v8::Context> context;
        *result = val->Uint32Value(context).FromJust();
    }

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetValueInt64(JSVM_Env env, JSVM_Value value, int64_t* result)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8 calls here cannot throw
    // JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);

    // This is still a fast path very likely to be taken.
    if (val->IsInt32()) {
        *result = val.As<v8::Int32>()->Value();
        return ClearLastError(env);
    }

    RETURN_STATUS_IF_FALSE(env, val->IsNumber(), JSVM_NUMBER_EXPECTED);

    // v8::Value::IntegerValue() converts NaN, +Inf, and -Inf to INT64_MIN,
    // inconsistent with v8::Value::Int32Value() which converts those values to 0.
    // Special-case all non-finite values to match that behavior.
    double doubleValue = val.As<v8::Number>()->Value();
    if (std::isfinite(doubleValue)) {
        // Empty context: https://github.com/nodejs/node/issues/14379
        v8::Local<v8::Context> context;
        *result = val->IntegerValue(context).FromJust();
    } else {
        *result = 0;
    }

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetValueBigintInt64(JSVM_Env env, JSVM_Value value, int64_t* result, bool* lossless)
{
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, result);
    CHECK_ARG(env, lossless);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);

    RETURN_STATUS_IF_FALSE(env, val->IsBigInt(), JSVM_BIGINT_EXPECTED);

    *result = val.As<v8::BigInt>()->Int64Value(lossless);

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetValueBigintUint64(JSVM_Env env, JSVM_Value value, uint64_t* result, bool* lossless)
{
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, result);
    CHECK_ARG(env, lossless);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);

    RETURN_STATUS_IF_FALSE(env, val->IsBigInt(), JSVM_BIGINT_EXPECTED);

    *result = val.As<v8::BigInt>()->Uint64Value(lossless);

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL
OH_JSVM_GetValueBigintWords(JSVM_Env env, JSVM_Value value, int* signBit, size_t* wordCount, uint64_t* words)
{
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, wordCount);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);

    RETURN_STATUS_IF_FALSE(env, val->IsBigInt(), JSVM_BIGINT_EXPECTED);

    v8::Local<v8::BigInt> big = val.As<v8::BigInt>();

    int wordCountInt = *wordCount;

    if (signBit == nullptr && words == nullptr) {
        wordCountInt = big->WordCount();
    } else {
        CHECK_ARG(env, signBit);
        CHECK_ARG(env, words);
        big->ToWordsArray(signBit, &wordCountInt, words);
    }

    *wordCount = wordCountInt;

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetValueBool(JSVM_Env env, JSVM_Value value, bool* result)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8 calls here cannot throw
    // JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    RETURN_STATUS_IF_FALSE(env, val->IsBoolean(), JSVM_BOOLEAN_EXPECTED);

    *result = val.As<v8::Boolean>()->Value();

    return ClearLastError(env);
}

// Copies a JavaScript string into a LATIN-1 string buffer. The result is the
// number of bytes (excluding the null terminator) copied into buf.
// A sufficient buffer size should be greater than the length of string,
// reserving space for null terminator.
// If bufsize is insufficient, the string will be truncated and null terminated.
// If buf is NULL, this method returns the length of the string (in bytes)
// via the result parameter.
// The result argument is optional unless buf is NULL.
JSVM_Status JSVM_CDECL
OH_JSVM_GetValueStringLatin1(JSVM_Env env, JSVM_Value value, char* buf, size_t bufsize, size_t* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, value);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    RETURN_STATUS_IF_FALSE(env, val->IsString(), JSVM_STRING_EXPECTED);

    if (!buf) {
        CHECK_ARG(env, result);
        *result = val.As<v8::String>()->Length();
    } else if (bufsize != 0) {
        int copied = val.As<v8::String>()->WriteOneByte(env->isolate, reinterpret_cast<uint8_t*>(buf), 0, bufsize - 1,
                                                        v8::String::NO_NULL_TERMINATION);

        buf[copied] = '\0';
        if (result != nullptr) {
            *result = copied;
        }
    } else if (result != nullptr) {
        *result = 0;
    }

    return ClearLastError(env);
}

// Copies a JavaScript string into a UTF-8 string buffer. The result is the
// number of bytes (excluding the null terminator) copied into buf.
// A sufficient buffer size should be greater than the length of string,
// reserving space for null terminator.
// If bufsize is insufficient, the string will be truncated and null terminated.
// If buf is NULL, this method returns the length of the string (in bytes)
// via the result parameter.
// The result argument is optional unless buf is NULL.
JSVM_Status JSVM_CDECL
OH_JSVM_GetValueStringUtf8(JSVM_Env env, JSVM_Value value, char* buf, size_t bufsize, size_t* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, value);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    RETURN_STATUS_IF_FALSE(env, val->IsString(), JSVM_STRING_EXPECTED);

    if (!buf) {
        CHECK_ARG(env, result);
        *result = val.As<v8::String>()->Utf8Length(env->isolate);
    } else if (bufsize != 0) {
        int copied =
            val.As<v8::String>()->WriteUtf8(env->isolate, buf, bufsize - 1, nullptr,
                                            v8::String::REPLACE_INVALID_UTF8 | v8::String::NO_NULL_TERMINATION);

        buf[copied] = '\0';
        if (result != nullptr) {
            *result = copied;
        }
    } else if (result != nullptr) {
        *result = 0;
    }

    return ClearLastError(env);
}

// Copies a JavaScript string into a UTF-16 string buffer. The result is the
// number of 2-byte code units (excluding the null terminator) copied into buf.
// A sufficient buffer size should be greater than the length of string,
// reserving space for null terminator.
// If bufsize is insufficient, the string will be truncated and null terminated.
// If buf is NULL, this method returns the length of the string (in 2-byte
// code units) via the result parameter.
// The result argument is optional unless buf is NULL.
JSVM_Status JSVM_CDECL
OH_JSVM_GetValueStringUtf16(JSVM_Env env, JSVM_Value value, char16_t* buf, size_t bufsize, size_t* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, value);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    RETURN_STATUS_IF_FALSE(env, val->IsString(), JSVM_STRING_EXPECTED);

    if (!buf) {
        CHECK_ARG(env, result);
        // V8 assumes UTF-16 length is the same as the number of characters.
        *result = val.As<v8::String>()->Length();
    } else if (bufsize != 0) {
        int copied = val.As<v8::String>()->Write(env->isolate, reinterpret_cast<uint16_t*>(buf), 0, bufsize - 1,
                                                 v8::String::NO_NULL_TERMINATION);

        buf[copied] = '\0';
        if (result != nullptr) {
            *result = copied;
        }
    } else if (result != nullptr) {
        *result = 0;
    }

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CoerceToBool(JSVM_Env env, JSVM_Value value, JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, result);

    v8::Isolate* isolate = env->isolate;
    v8::Local<v8::Boolean> b = v8impl::V8LocalValueFromJsValue(value)->ToBoolean(isolate);
    *result = v8impl::JsValueFromV8LocalValue(b);
    return GET_RETURN_STATUS(env);
}

#define GEN_COERCE_FUNCTION(UpperCaseName, MixedCaseName, LowerCaseName)                                       \
    JSVM_Status JSVM_CDECL OH_JSVM_CoerceTo##MixedCaseName(JSVM_Env env, JSVM_Value value, JSVM_Value* result) \
    {                                                                                                          \
        JSVM_PREAMBLE(env);                                                                                    \
        CHECK_ARG(env, value);                                                                                 \
        CHECK_ARG(env, result);                                                                                \
                                                                                                               \
        v8::Local<v8::Context> context = env->context();                                                       \
        v8::Local<v8::MixedCaseName> str;                                                                      \
                                                                                                               \
        CHECK_TO_##UpperCaseName(env, context, str, value);                                                    \
                                                                                                               \
        *result = v8impl::JsValueFromV8LocalValue(str);                                                        \
        return GET_RETURN_STATUS(env);                                                                         \
    }

GEN_COERCE_FUNCTION(NUMBER, Number, number)
GEN_COERCE_FUNCTION(OBJECT, Object, object)
GEN_COERCE_FUNCTION(STRING, String, string)
GEN_COERCE_FUNCTION(BIGINT, BigInt, bigint)

#undef GEN_COERCE_FUNCTION

JSVM_Status JSVM_CDECL OH_JSVM_Wrap(JSVM_Env env,
                                    JSVM_Value jsObject,
                                    void* nativeObject,
                                    JSVM_Finalize finalizeCb,
                                    void* finalizeHint,
                                    JSVM_Ref* result)
{
    return v8impl::Wrap(env, jsObject, nativeObject, finalizeCb, finalizeHint, result);
}

JSVM_Status JSVM_CDECL OH_JSVM_Unwrap(JSVM_Env env, JSVM_Value obj, void** result)
{
    return v8impl::Unwrap(env, obj, result, v8impl::KeepWrap);
}

JSVM_Status JSVM_CDECL OH_JSVM_RemoveWrap(JSVM_Env env, JSVM_Value obj, void** result)
{
    return v8impl::Unwrap(env, obj, result, v8impl::RemoveWrap);
}

JSVM_Status JSVM_CDECL
OH_JSVM_CreateExternal(JSVM_Env env, void* data, JSVM_Finalize finalizeCb, void* finalizeHint, JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, result);

    v8::Isolate* isolate = env->isolate;

    v8::Local<v8::Value> externalValue = v8::External::New(isolate, data);

    v8impl::RuntimeReference::New(env, externalValue, finalizeCb, data, finalizeHint);

    *result = v8impl::JsValueFromV8LocalValue(externalValue);

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_TypeTagObject(JSVM_Env env, JSVM_Value object, const JSVM_TypeTag* typeTag)
{
    JSVM_PREAMBLE(env);
    v8::Local<v8::Context> context = env->context();
    v8::Local<v8::Object> obj;
    CHECK_TO_OBJECT_WITH_PREAMBLE(env, context, obj, object);
    CHECK_ARG_WITH_PREAMBLE(env, typeTag);

    auto key = JSVM_PRIVATE_KEY(env->isolate, typeTag);
    auto maybeHas = obj->HasPrivate(context, key);
    CHECK_MAYBE_NOTHING_WITH_PREAMBLE(env, maybeHas, JSVM_GENERIC_FAILURE);
    RETURN_STATUS_IF_FALSE_WITH_PREAMBLE(env, !maybeHas.FromJust(), JSVM_INVALID_ARG);

    auto tag = v8::BigInt::NewFromWords(context, 0, 2, reinterpret_cast<const uint64_t*>(typeTag));
    CHECK_MAYBE_EMPTY_WITH_PREAMBLE(env, tag, JSVM_GENERIC_FAILURE);

    auto maybeSet = obj->SetPrivate(context, key, tag.ToLocalChecked());
    CHECK_MAYBE_NOTHING_WITH_PREAMBLE(env, maybeSet, JSVM_GENERIC_FAILURE);
    RETURN_STATUS_IF_FALSE_WITH_PREAMBLE(env, maybeSet.FromJust(), JSVM_GENERIC_FAILURE);

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CheckObjectTypeTag(JSVM_Env env,
                                                  JSVM_Value object,
                                                  const JSVM_TypeTag* typeTag,
                                                  bool* result)
{
    JSVM_PREAMBLE(env);
    v8::Local<v8::Context> context = env->context();
    v8::Local<v8::Object> obj;
    CHECK_TO_OBJECT_WITH_PREAMBLE(env, context, obj, object);
    CHECK_ARG_WITH_PREAMBLE(env, typeTag);
    CHECK_ARG_WITH_PREAMBLE(env, result);

    auto maybeValue = obj->GetPrivate(context, JSVM_PRIVATE_KEY(env->isolate, typeTag));
    CHECK_MAYBE_EMPTY_WITH_PREAMBLE(env, maybeValue, JSVM_GENERIC_FAILURE);
    v8::Local<v8::Value> val = maybeValue.ToLocalChecked();

    // We consider the type check to have failed unless we reach the line below
    // where we set whether the type check succeeded or not based on the
    // comparison of the two type tags.
    *result = false;
    if (val->IsBigInt()) {
        int sign;
        int size = 2;
        JSVM_TypeTag tag;
        val.As<v8::BigInt>()->ToWordsArray(&sign, &size, reinterpret_cast<uint64_t*>(&tag));
        if (sign == 0) {
            if (size == 2) {
                *result = (tag.lower == typeTag->lower && tag.upper == typeTag->upper);
            } else if (size == 1) {
                *result = (tag.lower == typeTag->lower && 0 == typeTag->upper);
            } else if (size == 0) {
                *result = (0 == typeTag->lower && 0 == typeTag->upper);
            }
        }
    }

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetValueExternal(JSVM_Env env, JSVM_Value value, void** result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    RETURN_STATUS_IF_FALSE(env, val->IsExternal(), JSVM_INVALID_ARG);

    v8::Local<v8::External> externalValue = val.As<v8::External>();
    *result = externalValue->Value();

    return ClearLastError(env);
}

// Set initialRefcount to 0 for a weak reference, >0 for a strong reference.
JSVM_Status JSVM_CDECL OH_JSVM_CreateReference(JSVM_Env env,
                                               JSVM_Value value,
                                               uint32_t initialRefcount,
                                               JSVM_Ref* result)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8 calls here cannot throw
    // JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> v8Value = v8impl::V8LocalValueFromJsValue(value);
    v8impl::UserReference* reference = v8impl::UserReference::New(env, v8Value, initialRefcount);

    *result = reinterpret_cast<JSVM_Ref>(reference);
    return ClearLastError(env);
}

// Deletes a reference. The referenced value is released, and may be GC'd unless
// there are other references to it.
JSVM_Status JSVM_CDECL OH_JSVM_DeleteReference(JSVM_Env env, JSVM_Ref ref)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8 calls here cannot throw
    // JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, ref);

    delete reinterpret_cast<v8impl::UserReference*>(ref);

    return ClearLastError(env);
}

// Increments the reference count, optionally returning the resulting count.
// After this call the reference will be a strong reference because its
// refcount is >0, and the referenced object is effectively "pinned".
// Calling this when the refcount is 0 and the object is unavailable
// results in an error.
JSVM_Status JSVM_CDECL OH_JSVM_ReferenceRef(JSVM_Env env, JSVM_Ref ref, uint32_t* result)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8 calls here cannot throw
    // JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, ref);

    v8impl::UserReference* reference = reinterpret_cast<v8impl::UserReference*>(ref);
    uint32_t count = reference->Ref();

    if (result != nullptr) {
        *result = count;
    }

    return ClearLastError(env);
}

// Decrements the reference count, optionally returning the resulting count. If
// the result is 0 the reference is now weak and the object may be GC'd at any
// time if there are no other references. Calling this when the refcount is
// already 0 results in an error.
JSVM_Status JSVM_CDECL OH_JSVM_ReferenceUnref(JSVM_Env env, JSVM_Ref ref, uint32_t* result)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8 calls here cannot throw
    // JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, ref);

    v8impl::UserReference* reference = reinterpret_cast<v8impl::UserReference*>(ref);

    if (reference->RefCount() == 0) {
        return SetLastError(env, JSVM_GENERIC_FAILURE);
    }

    uint32_t count = reference->Unref();

    if (result != nullptr) {
        *result = count;
    }

    return ClearLastError(env);
}

// Attempts to get a referenced value. If the reference is weak, the value might
// no longer be available, in that case the call is still successful but the
// result is NULL.
JSVM_Status JSVM_CDECL OH_JSVM_GetReferenceValue(JSVM_Env env, JSVM_Ref ref, JSVM_Value* result)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8 calls here cannot throw
    // JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, ref);
    CHECK_ARG(env, result);

    v8impl::UserReference* reference = reinterpret_cast<v8impl::UserReference*>(ref);
    *result = v8impl::JsValueFromV8LocalValue(reference->Get());

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_OpenHandleScope(JSVM_Env env, JSVM_HandleScope* result)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8 calls here cannot throw
    // JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, result);

    *result = v8impl::JsHandleScopeFromV8HandleScope(new v8impl::HandleScopeWrapper(env->isolate));
    env->openHandleScopes++;
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CloseHandleScope(JSVM_Env env, JSVM_HandleScope scope)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8 calls here cannot throw
    // JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, scope);
    if (env->openHandleScopes == 0) {
        return JSVM_HANDLE_SCOPE_MISMATCH;
    }

    env->ReleaseJsvmData();
    env->openHandleScopes--;
    delete v8impl::V8HandleScopeFromJsHandleScope(scope);
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_OpenEscapableHandleScope(JSVM_Env env, JSVM_EscapableHandleScope* result)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8 calls here cannot throw
    // JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, result);

    *result =
        v8impl::JsEscapableHandleScopeFromV8EscapableHandleScope(new v8impl::EscapableHandleScopeWrapper(env->isolate));
    env->openHandleScopes++;
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CloseEscapableHandleScope(JSVM_Env env, JSVM_EscapableHandleScope scope)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8 calls here cannot throw
    // JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, scope);
    if (env->openHandleScopes == 0) {
        return JSVM_HANDLE_SCOPE_MISMATCH;
    }

    delete v8impl::V8EscapableHandleScopeFromJsEscapableHandleScope(scope);
    env->openHandleScopes--;
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_EscapeHandle(JSVM_Env env,
                                            JSVM_EscapableHandleScope scope,
                                            JSVM_Value escapee,
                                            JSVM_Value* result)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8 calls here cannot throw
    // JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, scope);
    CHECK_ARG(env, escapee);
    CHECK_ARG(env, result);

    v8impl::EscapableHandleScopeWrapper* s = v8impl::V8EscapableHandleScopeFromJsEscapableHandleScope(scope);
    if (!s->IsEscapeCalled()) {
        *result = v8impl::JsValueFromV8LocalValue(s->Escape(v8impl::V8LocalValueFromJsValue(escapee)));
        return ClearLastError(env);
    }
    return SetLastError(env, JSVM_ESCAPE_CALLED_TWICE);
}

JSVM_Status JSVM_CDECL
OH_JSVM_NewInstance(JSVM_Env env, JSVM_Value constructor, size_t argc, const JSVM_Value* argv, JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, constructor);
    if (argc > 0) {
        CHECK_ARG(env, argv);
    }
    CHECK_ARG(env, result);

    v8::Local<v8::Context> context = env->context();

    v8::Local<v8::Function> ctor;
    CHECK_TO_FUNCTION(env, ctor, constructor);

    auto maybe =
        ctor->NewInstance(context, argc, reinterpret_cast<v8::Local<v8::Value>*>(const_cast<JSVM_Value*>(argv)));
    CHECK_MAYBE_EMPTY(env, maybe, JSVM_PENDING_EXCEPTION);

    *result = v8impl::JsValueFromV8LocalValue(maybe.ToLocalChecked());
    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_Instanceof(JSVM_Env env, JSVM_Value object, JSVM_Value constructor, bool* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, object);
    CHECK_ARG(env, result);

    *result = false;

    v8::Local<v8::Object> ctor;
    v8::Local<v8::Context> context = env->context();

    CHECK_TO_OBJECT(env, context, ctor, constructor);

    if (!ctor->IsFunction()) {
        OH_JSVM_ThrowTypeError(env, "ERR_NAPI_CONS_FUNCTION", "Constructor must be a function");

        return SetLastError(env, JSVM_FUNCTION_EXPECTED);
    }

    JSVM_Status status = JSVM_GENERIC_FAILURE;

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(object);
    auto maybeResult = val->InstanceOf(context, ctor);
    CHECK_MAYBE_NOTHING(env, maybeResult, status);
    *result = maybeResult.FromJust();
    return GET_RETURN_STATUS(env);
}

// Methods to support catching exceptions
JSVM_Status JSVM_CDECL OH_JSVM_IsExceptionPending(JSVM_Env env, bool* result)
{
    // JSVM_PREAMBLE is not used here: this function must execute when there is a
    // pending exception.
    CHECK_ENV(env);
    CHECK_ARG(env, result);

    *result = !env->lastException.IsEmpty();
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetAndClearLastException(JSVM_Env env, JSVM_Value* result)
{
    // JSVM_PREAMBLE is not used here: this function must execute when there is a
    // pending exception.
    CHECK_ENV(env);
    CHECK_ARG(env, result);

    if (env->lastException.IsEmpty()) {
        return OH_JSVM_GetUndefined(env, result);
    } else {
        *result = v8impl::JsValueFromV8LocalValue(v8::Local<v8::Value>::New(env->isolate, env->lastException));
        env->lastException.Reset();
    }

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsArraybuffer(JSVM_Env env, JSVM_Value value, bool* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    *result = val->IsArrayBuffer();

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateArraybuffer(JSVM_Env env, size_t byteLength, void** data, JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, result);

    v8::Isolate* isolate = env->isolate;
    v8::Local<v8::ArrayBuffer> buffer = v8::ArrayBuffer::New(isolate, byteLength);

    // Optionally return a pointer to the buffer's data, to avoid another call to
    // retrieve it.
    if (data != nullptr) {
        *data = buffer->Data();
    }

    *result = v8impl::JsValueFromV8LocalValue(buffer);
    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_AllocateArrayBufferBackingStoreData(size_t byteLength,
                                                                   JSVM_InitializedFlag initialized,
                                                                   void** data)
{
    if (!data) {
        return JSVM_INVALID_ARG;
    }
    auto allocator = v8impl::GetOrCreateDefaultArrayBufferAllocator();
    *data = (initialized == JSVM_ZERO_INITIALIZED) ? allocator->Allocate(byteLength)
                                                   : allocator->AllocateUninitialized(byteLength);
    return *data ? JSVM_OK : JSVM_GENERIC_FAILURE;
}

JSVM_Status JSVM_CDECL OH_JSVM_FreeArrayBufferBackingStoreData(void* data)
{
    if (!data) {
        return JSVM_INVALID_ARG;
    }
    auto allocator = v8impl::GetOrCreateDefaultArrayBufferAllocator();
    allocator->Free(data, JSVM_AUTO_LENGTH);
    return JSVM_OK;
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateArrayBufferFromBackingStoreData(JSVM_Env env,
                                                                     void* data,
                                                                     size_t backingStoreSize,
                                                                     size_t offset,
                                                                     size_t arrayBufferSize,
                                                                     JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, data);
    CHECK_ARG(env, result);
    CHECK_ARG_NOT_ZERO(env, backingStoreSize);
    CHECK_ARG_NOT_ZERO(env, arrayBufferSize);
    void* dataPtr = static_cast<uint8_t*>(data) + offset;
    RETURN_STATUS_IF_FALSE(env, offset + arrayBufferSize <= backingStoreSize, JSVM_INVALID_ARG);
    auto backingStore =
        v8::ArrayBuffer::NewBackingStore(dataPtr, arrayBufferSize, v8::BackingStore::EmptyDeleter, nullptr);
    v8::Local<v8::ArrayBuffer> arrayBuffer = v8::ArrayBuffer::New(env->isolate, std::move(backingStore));
    *result = v8impl::JsValueFromV8LocalValue(arrayBuffer);
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetArraybufferInfo(JSVM_Env env, JSVM_Value arraybuffer, void** data, size_t* byteLength)
{
    CHECK_ENV(env);
    CHECK_ARG(env, arraybuffer);

    v8::Local<v8::Value> value = v8impl::V8LocalValueFromJsValue(arraybuffer);
    RETURN_STATUS_IF_FALSE(env, value->IsArrayBuffer(), JSVM_INVALID_ARG);

    v8::Local<v8::ArrayBuffer> ab = value.As<v8::ArrayBuffer>();

    if (data != nullptr) {
        *data = ab->Data();
    }

    if (byteLength != nullptr) {
        *byteLength = ab->ByteLength();
    }

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsTypedarray(JSVM_Env env, JSVM_Value value, bool* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    *result = val->IsTypedArray();

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateTypedarray(JSVM_Env env,
                                                JSVM_TypedarrayType type,
                                                size_t length,
                                                JSVM_Value arraybuffer,
                                                size_t byteOffset,
                                                JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, arraybuffer);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> value = v8impl::V8LocalValueFromJsValue(arraybuffer);
    RETURN_STATUS_IF_FALSE(env, value->IsArrayBuffer(), JSVM_INVALID_ARG);

    v8::Local<v8::ArrayBuffer> buffer = value.As<v8::ArrayBuffer>();
    v8::Local<v8::TypedArray> typedArray;

    switch (type) {
        case JSVM_INT8_ARRAY:
            CREATE_TYPED_ARRAY(env, Int8Array, 1, buffer, byteOffset, length, typedArray);
            break;
        case JSVM_UINT8_ARRAY:
            CREATE_TYPED_ARRAY(env, Uint8Array, 1, buffer, byteOffset, length, typedArray);
            break;
        case JSVM_UINT8_CLAMPED_ARRAY:
            CREATE_TYPED_ARRAY(env, Uint8ClampedArray, 1, buffer, byteOffset, length, typedArray);
            break;
        case JSVM_INT16_ARRAY:
            CREATE_TYPED_ARRAY(env, Int16Array, 2, buffer, byteOffset, length, typedArray);
            break;
        case JSVM_UINT16_ARRAY:
            CREATE_TYPED_ARRAY(env, Uint16Array, 2, buffer, byteOffset, length, typedArray);
            break;
        case JSVM_INT32_ARRAY:
            CREATE_TYPED_ARRAY(env, Int32Array, 4, buffer, byteOffset, length, typedArray);
            break;
        case JSVM_UINT32_ARRAY:
            CREATE_TYPED_ARRAY(env, Uint32Array, 4, buffer, byteOffset, length, typedArray);
            break;
        case JSVM_FLOAT32_ARRAY:
            CREATE_TYPED_ARRAY(env, Float32Array, 4, buffer, byteOffset, length, typedArray);
            break;
        case JSVM_FLOAT64_ARRAY:
            CREATE_TYPED_ARRAY(env, Float64Array, 8, buffer, byteOffset, length, typedArray);
            break;
        case JSVM_BIGINT64_ARRAY:
            CREATE_TYPED_ARRAY(env, BigInt64Array, 8, buffer, byteOffset, length, typedArray);
            break;
        case JSVM_BIGUINT64_ARRAY:
            CREATE_TYPED_ARRAY(env, BigUint64Array, 8, buffer, byteOffset, length, typedArray);
            break;
        default:
            return SetLastError(env, JSVM_INVALID_ARG);
    }

    *result = v8impl::JsValueFromV8LocalValue(typedArray);
    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetTypedarrayInfo(JSVM_Env env,
                                                 JSVM_Value typedarray,
                                                 JSVM_TypedarrayType* type,
                                                 size_t* length,
                                                 void** data,
                                                 JSVM_Value* arraybuffer,
                                                 size_t* byteOffset)
{
    CHECK_ENV(env);
    CHECK_ARG(env, typedarray);

    v8::Local<v8::Value> value = v8impl::V8LocalValueFromJsValue(typedarray);
    RETURN_STATUS_IF_FALSE(env, value->IsTypedArray(), JSVM_INVALID_ARG);

    v8::Local<v8::TypedArray> array = value.As<v8::TypedArray>();

    if (type != nullptr) {
        if (value->IsInt8Array()) {
            *type = JSVM_INT8_ARRAY;
        } else if (value->IsUint8Array()) {
            *type = JSVM_UINT8_ARRAY;
        } else if (value->IsUint8ClampedArray()) {
            *type = JSVM_UINT8_CLAMPED_ARRAY;
        } else if (value->IsInt16Array()) {
            *type = JSVM_INT16_ARRAY;
        } else if (value->IsUint16Array()) {
            *type = JSVM_UINT16_ARRAY;
        } else if (value->IsInt32Array()) {
            *type = JSVM_INT32_ARRAY;
        } else if (value->IsUint32Array()) {
            *type = JSVM_UINT32_ARRAY;
        } else if (value->IsFloat32Array()) {
            *type = JSVM_FLOAT32_ARRAY;
        } else if (value->IsFloat64Array()) {
            *type = JSVM_FLOAT64_ARRAY;
        } else if (value->IsBigInt64Array()) {
            *type = JSVM_BIGINT64_ARRAY;
        } else if (value->IsBigUint64Array()) {
            *type = JSVM_BIGUINT64_ARRAY;
        }
    }

    if (length != nullptr) {
        *length = array->Length();
    }

    v8::Local<v8::ArrayBuffer> buffer;
    if (data != nullptr || arraybuffer != nullptr) {
        // Calling Buffer() may have the side effect of allocating the buffer,
        // so only do this when it’s needed.
        buffer = array->Buffer();
    }

    if (data != nullptr) {
        *data = static_cast<uint8_t*>(buffer->Data()) + array->ByteOffset();
    }

    if (arraybuffer != nullptr) {
        *arraybuffer = v8impl::JsValueFromV8LocalValue(buffer);
    }

    if (byteOffset != nullptr) {
        *byteOffset = array->ByteOffset();
    }

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL
OH_JSVM_CreateDataview(JSVM_Env env, size_t byteLength, JSVM_Value arraybuffer, size_t byteOffset, JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, arraybuffer);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> value = v8impl::V8LocalValueFromJsValue(arraybuffer);
    RETURN_STATUS_IF_FALSE(env, value->IsArrayBuffer(), JSVM_INVALID_ARG);

    v8::Local<v8::ArrayBuffer> buffer = value.As<v8::ArrayBuffer>();
    // TODO: should here throw an exception ?
    if (byteLength + byteOffset > buffer->ByteLength()) {
        OH_JSVM_ThrowRangeError(env, "ERR_JSVM_INVALID_DATAVIEW_ARGS",
                                "byteOffset + byteLength should be less than or "
                                "equal to the size in bytes of the array passed in");
        return SetLastError(env, JSVM_PENDING_EXCEPTION);
    }
    v8::Local<v8::DataView> DataView = v8::DataView::New(buffer, byteOffset, byteLength);

    *result = v8impl::JsValueFromV8LocalValue(DataView);
    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsDataview(JSVM_Env env, JSVM_Value value, bool* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    *result = val->IsDataView();

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetDataviewInfo(JSVM_Env env,
                                               JSVM_Value dataview,
                                               size_t* byteLength,
                                               void** data,
                                               JSVM_Value* arraybuffer,
                                               size_t* byteOffset)
{
    CHECK_ENV(env);
    CHECK_ARG(env, dataview);

    v8::Local<v8::Value> value = v8impl::V8LocalValueFromJsValue(dataview);
    RETURN_STATUS_IF_FALSE(env, value->IsDataView(), JSVM_INVALID_ARG);

    v8::Local<v8::DataView> array = value.As<v8::DataView>();

    if (byteLength != nullptr) {
        *byteLength = array->ByteLength();
    }

    v8::Local<v8::ArrayBuffer> buffer;
    if (data != nullptr || arraybuffer != nullptr) {
        // Calling Buffer() may have the side effect of allocating the buffer,
        // so only do this when it’s needed.
        buffer = array->Buffer();
    }

    if (data != nullptr) {
        *data = static_cast<uint8_t*>(buffer->Data()) + array->ByteOffset();
    }

    if (arraybuffer != nullptr) {
        *arraybuffer = v8impl::JsValueFromV8LocalValue(buffer);
    }

    if (byteOffset != nullptr) {
        *byteOffset = array->ByteOffset();
    }

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetVersion(JSVM_Env env, uint32_t* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, result);
    *result = JSVM_API_VERSION;
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreatePromise(JSVM_Env env, JSVM_Deferred* deferred, JSVM_Value* promise)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, deferred);
    CHECK_ARG(env, promise);

    auto maybe = v8::Promise::Resolver::New(env->context());
    CHECK_MAYBE_EMPTY(env, maybe, JSVM_GENERIC_FAILURE);

    auto resolver = maybe.ToLocalChecked();
    auto v8Deferred = new v8impl::Persistent<v8::Value>();
    v8Deferred->Reset(env->isolate, resolver);

    *deferred = v8impl::JsDeferredFromPersistent(v8Deferred);
    *promise = v8impl::JsValueFromV8LocalValue(resolver->GetPromise());
    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_ResolveDeferred(JSVM_Env env, JSVM_Deferred deferred, JSVM_Value resolution)
{
    return v8impl::ConcludeDeferred(env, deferred, resolution, true);
}

JSVM_Status JSVM_CDECL OH_JSVM_RejectDeferred(JSVM_Env env, JSVM_Deferred deferred, JSVM_Value resolution)
{
    return v8impl::ConcludeDeferred(env, deferred, resolution, false);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsPromise(JSVM_Env env, JSVM_Value value, bool* isPromise)
{
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, isPromise);

    *isPromise = v8impl::V8LocalValueFromJsValue(value)->IsPromise();

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateDate(JSVM_Env env, double time, JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, result);

    v8::MaybeLocal<v8::Value> maybeDate = v8::Date::New(env->context(), time);
    CHECK_MAYBE_EMPTY(env, maybeDate, JSVM_GENERIC_FAILURE);

    *result = v8impl::JsValueFromV8LocalValue(maybeDate.ToLocalChecked());

    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsDate(JSVM_Env env, JSVM_Value value, bool* isDate)
{
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, isDate);

    *isDate = v8impl::V8LocalValueFromJsValue(value)->IsDate();

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetDateValue(JSVM_Env env, JSVM_Value value, double* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    RETURN_STATUS_IF_FALSE(env, val->IsDate(), JSVM_DATE_EXPECTED);

    v8::Local<v8::Date> date = val.As<v8::Date>();
    *result = date->ValueOf();

    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_AddFinalizer(JSVM_Env env,
                                            JSVM_Value jsObject,
                                            void* finalizeData,
                                            JSVM_Finalize finalizeCb,
                                            void* finalizeHint,
                                            JSVM_Ref* result)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8 calls here cannot throw
    // JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, jsObject);
    CHECK_ARG(env, finalizeCb);

    v8::Local<v8::Value> v8Value = v8impl::V8LocalValueFromJsValue(jsObject);
    RETURN_STATUS_IF_FALSE(env, v8Value->IsObject(), JSVM_INVALID_ARG);

    v8impl::RuntimeReference::New(env, v8Value, finalizeCb, finalizeData, finalizeHint);

    if (result != nullptr) {
        auto* ref = v8impl::UserReference::New(env, v8Value, 0);
        *result = reinterpret_cast<JSVM_Ref>(ref);
    }
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_AdjustExternalMemory(JSVM_Env env, int64_t changeInBytes, int64_t* adjustedValue)
{
    CHECK_ENV(env);
    CHECK_ARG(env, adjustedValue);

    *adjustedValue = env->isolate->AdjustAmountOfExternalAllocatedMemory(changeInBytes);

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_SetInstanceData(JSVM_Env env, void* data, JSVM_Finalize finalizeCb, void* finalizeHint)
{
    CHECK_ENV(env);

    v8impl::FinalizerTracker* oldData = static_cast<v8impl::FinalizerTracker*>(env->instanceData);
    if (oldData != nullptr) {
        // Our contract so far has been to not finalize any old data there may be.
        // So we simply delete it.
        delete oldData;
    }

    env->instanceData = v8impl::FinalizerTracker::New(env, finalizeCb, data, finalizeHint);

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_GetInstanceData(JSVM_Env env, void** data)
{
    CHECK_ENV(env);
    CHECK_ARG(env, data);

    v8impl::FinalizerTracker* idata = static_cast<v8impl::FinalizerTracker*>(env->instanceData);

    *data = (idata == nullptr ? nullptr : idata->GetData());

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_DetachArraybuffer(JSVM_Env env, JSVM_Value arraybuffer)
{
    CHECK_ENV(env);
    CHECK_ARG(env, arraybuffer);

    v8::Local<v8::Value> value = v8impl::V8LocalValueFromJsValue(arraybuffer);
    RETURN_STATUS_IF_FALSE(env, value->IsArrayBuffer() || value->IsSharedArrayBuffer(), JSVM_ARRAYBUFFER_EXPECTED);

    v8::Local<v8::ArrayBuffer> it = value.As<v8::ArrayBuffer>();
    RETURN_STATUS_IF_FALSE(env, it->IsDetachable(), JSVM_DETACHABLE_ARRAYBUFFER_EXPECTED);

    it->Detach();

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsDetachedArraybuffer(JSVM_Env env, JSVM_Value arraybuffer, bool* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, arraybuffer);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> value = v8impl::V8LocalValueFromJsValue(arraybuffer);

    *result = value->IsArrayBuffer() && value.As<v8::ArrayBuffer>()->WasDetached();

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_DefineClassWithPropertyHandler(JSVM_Env env,
                                                              const char* utf8name,
                                                              size_t length,
                                                              JSVM_Callback constructor,
                                                              size_t propertyCount,
                                                              const JSVM_PropertyDescriptor* properties,
                                                              JSVM_PropertyHandlerCfg propertyHandlerCfg,
                                                              JSVM_Callback callAsFunctionCallback,
                                                              JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, result);
    CHECK_ARG(env, constructor);
    CHECK_ARG(env, constructor->callback);
    CHECK_ARG(env, propertyHandlerCfg);

    if (propertyCount > 0) {
        CHECK_ARG(env, properties);
    }

    v8::Isolate* isolate = env->isolate;
    v8::EscapableHandleScope scope(isolate);
    v8::Local<v8::FunctionTemplate> tpl;
    STATUS_CALL(v8impl::FunctionCallbackWrapper::NewTemplate(env, constructor, &tpl));

    v8::Local<v8::String> nameString;
    CHECK_NEW_FROM_UTF8_LEN(env, nameString, utf8name, length);
    tpl->SetClassName(nameString);

    size_t staticPropertyCount = 0;
    for (size_t i = 0; i < propertyCount; i++) {
        const JSVM_PropertyDescriptor* p = properties + i;

        if ((p->attributes & JSVM_STATIC) != 0) { // attributes
            // Static properties are handled separately below.
            staticPropertyCount++;
            continue;
        }

        v8::Local<v8::Name> propertyName;
        STATUS_CALL(v8impl::V8NameFromPropertyDescriptor(env, p, &propertyName));
        v8::PropertyAttribute attributes = v8impl::V8PropertyAttributesFromDescriptor(p);

        // This code is similar to that in OH_JSVM_DefineProperties(); the
        // difference is it applies to a template instead of an object,
        // and preferred PropertyAttribute for lack of PropertyDescriptor
        // support on ObjectTemplate.
        if (p->getter != nullptr || p->setter != nullptr) {
            v8::Local<v8::FunctionTemplate> getterTpl;
            v8::Local<v8::FunctionTemplate> setterTpl;
            if (p->getter != nullptr) {
                STATUS_CALL(v8impl::FunctionCallbackWrapper::NewTemplate(env, p->getter, &getterTpl));
            }
            if (p->setter != nullptr) {
                STATUS_CALL(v8impl::FunctionCallbackWrapper::NewTemplate(env, p->setter, &setterTpl));
            }

            tpl->PrototypeTemplate()->SetAccessorProperty(propertyName, getterTpl, setterTpl, attributes,
                                                          v8::AccessControl::DEFAULT);
        } else if (p->method != nullptr) {
            v8::Local<v8::FunctionTemplate> t;
            if (p->attributes & JSVM_NO_RECEIVER_CHECK) {
                STATUS_CALL(v8impl::FunctionCallbackWrapper::NewTemplate(env, p->method, &t));
            } else {
                STATUS_CALL(
                    v8impl::FunctionCallbackWrapper::NewTemplate(env, p->method, &t, v8::Signature::New(isolate, tpl)));
            }

            tpl->PrototypeTemplate()->Set(propertyName, t, attributes);
        } else {
            v8::Local<v8::Value> value = v8impl::V8LocalValueFromJsValue(p->value);
            tpl->PrototypeTemplate()->Set(propertyName, value, attributes);
        }
    }

    /* register property handler for instance object */
    v8impl::JSVM_PropertyHandlerCfgStruct* propertyHandleCfg = v8impl::CreatePropertyCfg(env, propertyHandlerCfg);
    if (propertyHandleCfg == nullptr) {
        return JSVM_Status::JSVM_GENERIC_FAILURE;
    }
    v8::Local<v8::Value> cbdata = v8impl::CallbackBundle::New(env, propertyHandleCfg);

    // register named property handler
    v8::NamedPropertyHandlerConfiguration namedPropertyHandler;
    if (propertyHandlerCfg->genericNamedPropertyGetterCallback) {
        namedPropertyHandler.getter = v8impl::PropertyCallbackWrapper<v8::Value>::NameGetterInvoke;
    }
    if (propertyHandlerCfg->genericNamedPropertySetterCallback) {
        namedPropertyHandler.setter = v8impl::PropertyCallbackWrapper<v8::Value>::NameSetterInvoke;
    }
    if (propertyHandlerCfg->genericNamedPropertyDeleterCallback) {
        namedPropertyHandler.deleter = v8impl::PropertyCallbackWrapper<v8::Boolean>::NameDeleterInvoke;
    }
    if (propertyHandlerCfg->genericNamedPropertyEnumeratorCallback) {
        namedPropertyHandler.enumerator = v8impl::PropertyCallbackWrapper<v8::Array>::NameEnumeratorInvoke;
    }
    namedPropertyHandler.data = cbdata;
    tpl->InstanceTemplate()->SetHandler(namedPropertyHandler);

    // register indexed property handle
    v8::IndexedPropertyHandlerConfiguration indexPropertyHandler;
    if (propertyHandlerCfg->genericIndexedPropertyGetterCallback) {
        indexPropertyHandler.getter = v8impl::PropertyCallbackWrapper<v8::Value>::IndexGetterInvoke;
    }
    if (propertyHandlerCfg->genericIndexedPropertySetterCallback) {
        indexPropertyHandler.setter = v8impl::PropertyCallbackWrapper<v8::Value>::IndexSetterInvoke;
    }
    if (propertyHandlerCfg->genericIndexedPropertyDeleterCallback) {
        indexPropertyHandler.deleter = v8impl::PropertyCallbackWrapper<v8::Boolean>::IndexDeleterInvoke;
    }
    if (propertyHandlerCfg->genericIndexedPropertyEnumeratorCallback) {
        indexPropertyHandler.enumerator = v8impl::PropertyCallbackWrapper<v8::Array>::IndexEnumeratorInvoke;
    }
    indexPropertyHandler.data = cbdata;
    tpl->InstanceTemplate()->SetHandler(indexPropertyHandler);

    // register call as function
    if (callAsFunctionCallback && callAsFunctionCallback->callback) {
        v8::Local<v8::Value> funcCbdata = v8impl::CallbackBundle::New(env, callAsFunctionCallback);
        tpl->InstanceTemplate()->SetCallAsFunctionHandler(v8impl::FunctionCallbackWrapper::Invoke, funcCbdata);
    }

    v8::Local<v8::Context> context = env->context();
    *result = v8impl::JsValueFromV8LocalValue(scope.Escape(tpl->GetFunction(context).ToLocalChecked()));

    v8impl::RuntimeReference::New(env, v8impl::V8LocalValueFromJsValue(*result), v8impl::CfgFinalizedCallback,
                                  propertyHandleCfg, nullptr);

    if (staticPropertyCount > 0) {
        std::vector<JSVM_PropertyDescriptor> staticDescriptors;
        staticDescriptors.reserve(staticPropertyCount);

        for (size_t i = 0; i < propertyCount; i++) {
            const JSVM_PropertyDescriptor* p = properties + i;
            if ((p->attributes & JSVM_STATIC) != 0) {
                staticDescriptors.push_back(*p);
            }
        }

        STATUS_CALL(OH_JSVM_DefineProperties(env, *result, staticDescriptors.size(), staticDescriptors.data()));
    }

    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsLocked(JSVM_Env env, bool* isLocked)
{
    CHECK_ENV(env);
    CHECK_ARG(env, isLocked);

    *isLocked = v8::Locker::IsLocked(env->isolate);

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_AcquireLock(JSVM_Env env)
{
    CHECK_ENV(env);

    bool isLocked = v8::Locker::IsLocked(env->isolate);
    if (!isLocked) {
        env->locker = new v8::Locker(env->isolate);
    }

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_ReleaseLock(JSVM_Env env)
{
    CHECK_ENV(env);

    bool isLocked = v8::Locker::IsLocked(env->isolate);
    if (isLocked && env->locker != nullptr) {
        delete env->locker;
        env->locker = nullptr;
    }

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsCallable(JSVM_Env env, JSVM_Value value, bool* isCallable)
{
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, isCallable);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);

    *isCallable = val->IsFunction();
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsUndefined(JSVM_Env env, JSVM_Value value, bool* isUndefined)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8
    // calls here cannot throw JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, isUndefined);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    *isUndefined = val->IsUndefined();

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsNull(JSVM_Env env, JSVM_Value value, bool* isNull)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8
    // calls here cannot throw JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, isNull);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    *isNull = val->IsNull();

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsNullOrUndefined(JSVM_Env env, JSVM_Value value, bool* isNullOrUndefined)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8
    // calls here cannot throw JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, isNullOrUndefined);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    *isNullOrUndefined = val->IsNullOrUndefined();

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsBoolean(JSVM_Env env, JSVM_Value value, bool* isBoolean)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8
    // calls here cannot throw JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, isBoolean);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    *isBoolean = val->IsBoolean();

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsNumber(JSVM_Env env, JSVM_Value value, bool* isNumber)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8
    // calls here cannot throw JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, isNumber);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    *isNumber = val->IsNumber();

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsString(JSVM_Env env, JSVM_Value value, bool* isString)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8
    // calls here cannot throw JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, isString);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    *isString = val->IsString();

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsSymbol(JSVM_Env env, JSVM_Value value, bool* isSymbol)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8
    // calls here cannot throw JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, isSymbol);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    *isSymbol = val->IsSymbol();

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsFunction(JSVM_Env env, JSVM_Value value, bool* isFunction)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8
    // calls here cannot throw JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, isFunction);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    *isFunction = val->IsFunction();

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsObject(JSVM_Env env, JSVM_Value value, bool* isObject)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8
    // calls here cannot throw JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, isObject);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    *isObject = val->IsObject();

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsBigInt(JSVM_Env env, JSVM_Value value, bool* isBigInt)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8
    // calls here cannot throw JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, isBigInt);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    *isBigInt = val->IsBigInt();

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsConstructor(JSVM_Env env, JSVM_Value value, bool* isConstructor)
{
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, isConstructor);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    if (!val->IsObject()) {
        *isConstructor = false;
        return ClearLastError(env);
    }
    v8::Local<v8::Object> obj = val.As<v8::Object>();
    *isConstructor = obj->IsConstructor();

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateRegExp(JSVM_Env env, JSVM_Value value, JSVM_RegExpFlags flags, JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> pattern = v8impl::V8LocalValueFromJsValue(value);
    RETURN_STATUS_IF_FALSE(env, pattern->IsString(), JSVM_STRING_EXPECTED);
    v8::Local<v8::Context> context = env->context();
    v8::MaybeLocal<v8::RegExp> regExp =
        v8::RegExp::New(context, pattern.As<v8::String>(), static_cast<v8::RegExp::Flags>(flags));
    CHECK_MAYBE_EMPTY(env, regExp, JSVM_GENERIC_FAILURE);
    *result = v8impl::JsValueFromV8LocalValue(regExp.ToLocalChecked());

    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateMap(JSVM_Env env, JSVM_Value* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, result);

    *result = v8impl::JsValueFromV8LocalValue(v8::Map::New(env->isolate));

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsMap(JSVM_Env env, JSVM_Value value, bool* isMap)
{
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, isMap);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);

    *isMap = val->IsMap();
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_RetainScript(JSVM_Env env, JSVM_Script script)
{
    CHECK_ENV(env);
    auto jsvmData = reinterpret_cast<JSVM_Data__*>(script);

    RETURN_STATUS_IF_FALSE(env, jsvmData && !jsvmData->isGlobal, JSVM_INVALID_ARG);

    jsvmData->taggedPointer = v8::Global<v8::Script>(env->isolate, jsvmData->ToV8Local<v8::Script>(env->isolate));

    jsvmData->isGlobal = true;
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_ReleaseScript(JSVM_Env env, JSVM_Script script)
{
    CHECK_ENV(env);
    auto jsvmData = reinterpret_cast<JSVM_Data__*>(script);

    RETURN_STATUS_IF_FALSE(env, jsvmData && jsvmData->isGlobal, JSVM_INVALID_ARG);

    std::get<v8::Global<v8::Script>>(jsvmData->taggedPointer).Reset();
    delete jsvmData;
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_OpenInspectorWithName(JSVM_Env env, int pid, const char* name)
{
    JSVM_PREAMBLE(env);
    RETURN_STATUS_IF_FALSE(env, !name || strlen(name) < SIZE_MAX, JSVM_INVALID_ARG);
    RETURN_STATUS_IF_FALSE(env, pid >= 0, JSVM_INVALID_ARG);
    std::string path(name ? name : "jsvm");

    if (!env->GetInspectorAgent()->Start(path, pid)) {
        LOG(Error) << "Open Inspector failed: Please check the internet permisson.";
        return SetLastError(env, JSVM_GENERIC_FAILURE);
    }
    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateSet(JSVM_Env env, JSVM_Value* result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, result);

    *result = v8impl::JsValueFromV8LocalValue(v8::Set::New(env->isolate));

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsSet(JSVM_Env env, JSVM_Value value, bool* isSet)
{
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, isSet);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    *isSet = val->IsSet();

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_ObjectGetPrototypeOf(JSVM_Env env, JSVM_Value object, JSVM_Value* result)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, result);

    v8::Local<v8::Context> context = env->context();

    v8::Local<v8::Object> obj;
    CHECK_TO_OBJECT(env, context, obj, object);

    v8::Local<v8::Value> val = obj->GetPrototypeV2();
    *result = v8impl::JsValueFromV8LocalValue(val);
    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_ObjectSetPrototypeOf(JSVM_Env env, JSVM_Value object, JSVM_Value prototype)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, prototype);

    v8::Local<v8::Context> context = env->context();

    v8::Local<v8::Object> obj;
    CHECK_TO_OBJECT(env, context, obj, object);

    v8::Local<v8::Value> type = v8impl::V8LocalValueFromJsValue(prototype);
    RETURN_STATUS_IF_FALSE(env, type->IsObject(), JSVM_INVALID_ARG);
    v8::Maybe<bool> setMaybe = obj->SetPrototypeV2(context, type);

    RETURN_STATUS_IF_FALSE(env, setMaybe.FromMaybe(false), JSVM_GENERIC_FAILURE);
    return GET_RETURN_STATUS((env));
}

JSVM_Status JSVM_CDECL OH_JSVM_CompileWasmModule(JSVM_Env env,
                                                 const uint8_t* wasmBytecode,
                                                 size_t wasmBytecodeLength,
                                                 const uint8_t* cacheData,
                                                 size_t cacheDataLength,
                                                 bool* cacheRejected,
                                                 JSVM_Value* wasmModule)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, wasmBytecode);
    RETURN_STATUS_IF_FALSE(env, wasmBytecodeLength > 0, JSVM_INVALID_ARG);
    v8::MaybeLocal<v8::WasmModuleObject> maybeModule;
    if (cacheData == nullptr) {
        maybeModule = v8::WasmModuleObject::Compile(env->isolate, { wasmBytecode, wasmBytecodeLength });
    } else {
        RETURN_STATUS_IF_FALSE(env, cacheDataLength > 0, JSVM_INVALID_ARG);
        bool rejected;
        maybeModule = v8::WasmModuleObject::DeserializeOrCompile(env->isolate, { wasmBytecode, wasmBytecodeLength },
                                                                 { cacheData, cacheDataLength }, rejected);
        if (cacheRejected != nullptr) {
            *cacheRejected = rejected;
        }
    }
    // To avoid the status code caused by exception being override, check exception once v8 API finished
    RETURN_IF_EXCEPTION_HAS_CAUGHT(env);
    CHECK_MAYBE_EMPTY(env, maybeModule, JSVM_GENERIC_FAILURE);
    *wasmModule = v8impl::JsValueFromV8LocalValue(maybeModule.ToLocalChecked());
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CompileWasmFunction(JSVM_Env env,
                                                   JSVM_Value wasmModule,
                                                   uint32_t functionIndex,
                                                   JSVM_WasmOptLevel optLevel)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, wasmModule);
    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(wasmModule);
    RETURN_STATUS_IF_FALSE(env, val->IsWasmModuleObject(), JSVM_INVALID_ARG);

    v8::Local<v8::WasmModuleObject> v8WasmModule = val.As<v8::WasmModuleObject>();
    v8::WasmExecutionTier tier = v8::WasmExecutionTier::kNone;
    if (optLevel == JSVM_WASM_OPT_BASELINE) {
        // v8 liftoff has bug, keep BASELINE same as HIGH.
        tier = v8::WasmExecutionTier::kTurbofan;
    } else if (optLevel == JSVM_WASM_OPT_HIGH) {
        tier = v8::WasmExecutionTier::kTurbofan;
    } else {
        // Unsupported optLevel
        return SetLastError(env, JSVM_INVALID_ARG);
    }
    bool compileSuccess = v8WasmModule->CompileFunction(env->isolate, functionIndex, tier);
    // To avoid the status code caused by exception being override, check exception once v8 API finished
    RETURN_IF_EXCEPTION_HAS_CAUGHT(env);
    RETURN_STATUS_IF_FALSE(env, compileSuccess, JSVM_GENERIC_FAILURE);
    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_IsWasmModuleObject(JSVM_Env env, JSVM_Value value, bool* result)
{
    // Omit JSVM_PREAMBLE and GET_RETURN_STATUS because V8
    // calls here cannot throw JS exceptions.
    CHECK_ENV(env);
    CHECK_ARG(env, value);
    CHECK_ARG(env, result);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
    *result = val->IsWasmModuleObject();

    return ClearLastError(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_CreateWasmCache(JSVM_Env env,
                                               JSVM_Value wasmModule,
                                               const uint8_t** data,
                                               size_t* length)
{
    JSVM_PREAMBLE(env);
    CHECK_ARG(env, wasmModule);
    CHECK_ARG(env, data);
    CHECK_ARG(env, length);

    v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(wasmModule);
    RETURN_STATUS_IF_FALSE(env, val->IsWasmModuleObject(), JSVM_INVALID_ARG);

    v8::Local<v8::WasmModuleObject> v8WasmModule = val.As<v8::WasmModuleObject>();
    v8::CompiledWasmModule compiledWasmModule = v8WasmModule->GetCompiledModule();
    v8::OwnedBuffer serializedBytes = compiledWasmModule.Serialize();
    // To avoid the status code caused by exception being override, check exception once v8 API finished
    RETURN_IF_EXCEPTION_HAS_CAUGHT(env);
    // If buffer size is 0, create wasm cache failed.
    RETURN_STATUS_IF_FALSE(env, serializedBytes.size > 0, JSVM_GENERIC_FAILURE);
    *data = serializedBytes.buffer.get();
    *length = serializedBytes.size;
    // Release the ownership of buffer, OH_JSVM_ReleaseCache must be called explicitly to release the buffer
    serializedBytes.buffer.release();

    return GET_RETURN_STATUS(env);
}

JSVM_Status JSVM_CDECL OH_JSVM_ReleaseCache(JSVM_Env env, const uint8_t* cacheData, JSVM_CacheType cacheType)
{
    CHECK_ENV(env);
    CHECK_ARG(env, cacheData);
    if (cacheType == JSVM_CACHE_TYPE_JS) {
        // The release behavior MUST match the memory allocation of OH_JSVM_CreateCodeCache.
        delete[] cacheData;
    } else if (cacheType == JSVM_CACHE_TYPE_WASM) {
        // The release behavior MUST match the memory allocation of OH_JSVM_CreateWasmCache.
        delete[] cacheData;
    } else {
        // Unsupported cacheType
        return SetLastError(env, JSVM_INVALID_ARG);
    }
    return ClearLastError(env);
}
