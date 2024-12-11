#ifndef SRC_JSVM_REFERENCE_
#define SRC_JSVM_REFERENCE_
#include <stdint.h>

#include "jsvm_types.h"
#include "jsvm_util.h"

namespace v8impl {
class RefTracker;
using RefList = RefTracker;

class RefTracker {
public:
    RefTracker() : next(nullptr), prev(nullptr) {}

    virtual ~RefTracker() = default;

    static void FinalizeAll(RefList* list);

protected:
    virtual void Finalize();

    inline void Link(RefList* list);

    inline void Unlink();

private:
    RefList* next;
    RefList* prev;
};

class UserReference final : public RefTracker {
public:
    static UserReference* New(JSVM_Env env, v8::Local<v8::Value> value, uint32_t initialRefcount);
    ~UserReference() override;

    // Increase and decrease reference
    uint32_t Ref();
    uint32_t Unref();
    uint32_t RefCount();

    // Get v8::Local value
    v8::Local<v8::Value> Get();

protected:
    UserReference(JSVM_Env env, v8::Local<v8::Value> value, uint32_t initialRefcount);

    void Finalize() override;

private:
    void SetWeak();

private:
    v8impl::Persistent<v8::Value> persistent;
    JSVM_Env env;
    uint32_t refcount;
    bool canBeWeak;
};

class FinalizerTracker : public RefTracker {
protected:
    FinalizerTracker(JSVM_Env env, JSVM_Finalize cb, void* data, void* hint);

public:
    static FinalizerTracker* New(JSVM_Env env, JSVM_Finalize cb, void* data, void* hint);

    ~FinalizerTracker() override;

    void* GetData()
    {
        return data;
    }

protected:
    void ResetFinalizer();

    void CallFinalizer();

    void Finalize() override;

private:
    JSVM_Env env;
    JSVM_Finalize cb;
    void* data;
    void* hint;
};

class RuntimeReference : public FinalizerTracker {
protected:
    RuntimeReference(JSVM_Env env, v8::Local<v8::Value> value, JSVM_Finalize cb, void* data, void* hint);

public:
    static RuntimeReference* New(JSVM_Env env, v8::Local<v8::Value> value, void* data);
    static RuntimeReference* New(JSVM_Env env, v8::Local<v8::Value> value, JSVM_Finalize cb, void* data, void* hint);
    static void DeleteReference(RuntimeReference* ref);

private:
    inline void SetWeak(bool needSecondPass);
    static void FirstPassCallback(const v8::WeakCallbackInfo<RuntimeReference>& data);
    static void SecondPassCallback(const v8::WeakCallbackInfo<RuntimeReference>& data);
    static void FirstPassCallbackWithoutFinalizer(const v8::WeakCallbackInfo<RuntimeReference>& data);

private:
    v8impl::Persistent<v8::Value> persistent;
};

} // namespace v8impl

#endif