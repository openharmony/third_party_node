#ifndef JSVM_VALUE_H
#define JSVM_VALUE_H
#include <variant>

#include "jsvm_util.h"

struct JSVM_Data__ {
public:
    using SourcePtr = std::variant<v8::Local<v8::Script>, v8::Global<v8::Script>>;
    using DataType = enum { kJsvmScript };

    template<typename T>
    JSVM_Data__(T ptr, bool retained, DataType type = kJsvmScript) : taggedPointer(ptr), isGlobal(retained), type(type)
    {}

    template<class T>
    v8::Local<T> ToV8Local(v8::Isolate* isolate)
    {
        if (isGlobal) {
            return v8::Local<T>::New(isolate, std::get<v8::Global<T>>(taggedPointer));
        } else {
            return std::get<v8::Local<T>>(taggedPointer);
        }
    }

    SourcePtr taggedPointer;
    bool isGlobal = false;
    DataType type;
};

namespace v8impl {
//=== Conversion between V8 Handles and JSVM_Value ========================

// This asserts v8::Local<> will always be implemented with a single
// pointer field so that we can pass it around as a void*.
static_assert(sizeof(v8::Local<v8::Value>) == sizeof(JSVM_Value),
              "Cannot convert between v8::Local<v8::Value> and JSVM_Value");

inline JSVM_Value JsValueFromV8LocalValue(v8::Local<v8::Value> local)
{
    return reinterpret_cast<JSVM_Value>(*local);
}

inline v8::Local<v8::Value> V8LocalValueFromJsValue(JSVM_Value v)
{
    v8::Local<v8::Value> local;
    memcpy(static_cast<void*>(&local), &v, sizeof(v));
    return local;
}

inline JSVM_Deferred JsDeferredFromPersistent(v8impl::Persistent<v8::Value>* local)
{
    return reinterpret_cast<JSVM_Deferred>(local);
}

inline v8impl::Persistent<v8::Value>* PersistentFromJsDeferred(JSVM_Deferred local)
{
    return reinterpret_cast<v8impl::Persistent<v8::Value>*>(local);
}

class HandleScopeWrapper {
public:
    explicit HandleScopeWrapper(v8::Isolate* isolate) : scope(isolate) {}

private:
    v8::HandleScope scope;
};

class EscapableHandleScopeWrapper {
public:
    explicit EscapableHandleScopeWrapper(v8::Isolate* isolate) : scope(isolate), escapeCalled(false) {}
    bool IsEscapeCalled() const
    {
        return escapeCalled;
    }
    template<typename T>
    v8::Local<T> Escape(v8::Local<T> handle)
    {
        escapeCalled = true;
        return scope.Escape(handle);
    }

private:
    v8::EscapableHandleScope scope;
    bool escapeCalled;
};

inline JSVM_HandleScope JsHandleScopeFromV8HandleScope(HandleScopeWrapper* s)
{
    return reinterpret_cast<JSVM_HandleScope>(s);
}

inline HandleScopeWrapper* V8HandleScopeFromJsHandleScope(JSVM_HandleScope s)
{
    return reinterpret_cast<HandleScopeWrapper*>(s);
}

inline JSVM_EscapableHandleScope JsEscapableHandleScopeFromV8EscapableHandleScope(EscapableHandleScopeWrapper* s)
{
    return reinterpret_cast<JSVM_EscapableHandleScope>(s);
}

inline EscapableHandleScopeWrapper* V8EscapableHandleScopeFromJsEscapableHandleScope(JSVM_EscapableHandleScope s)
{
    return reinterpret_cast<EscapableHandleScopeWrapper*>(s);
}

} // namespace v8impl

#endif