#include "jsvm_env.h"

#include "libplatform/libplatform.h"

void JSVM_Env__::RunAndClearInterrupts()
{
    while (messageQueue.size() > 0) {
        std::vector<Callback> messageQueueTmp {};
        {
            const std::lock_guard<std::mutex> lock(messageQueueMutex);
            messageQueueTmp.swap(messageQueue);
        }
        jsvm::DebugSealHandleScope sealHandleScope(isolate);

        for (auto& cb : messageQueueTmp) {
            cb(this);
        }
    }
}

JSVM_Env__::JSVM_Env__(v8::Isolate* isolate, int32_t apiVersion) : isolate(isolate), apiVersion(apiVersion)
{
    inspectorAgent = jsvm::InspectorAgent::New(this);
    ClearLastError(this);
}

void JSVM_Env__::DeleteMe()
{
    v8impl::RefTracker::FinalizeAll(&finalizerList);
    v8impl::RefTracker::FinalizeAll(&userReferenceList);

    {
        v8::Context::Scope context_scope(context());
        if (inspectorAgent->IsActive()) {
            inspectorAgent->WaitForDisconnect();
        }
        delete inspectorAgent;
    }

    // release lock
    if (locker) {
        delete locker;
        locker = nullptr;
    }

    delete this;
}

#ifndef ENABLE_INSPECTOR
#include "jsvm_log.h"
namespace {
/*
 * If inspector is not enabled, using fake jsvm inspect agent.
 * All Interface in fake agent log error.
 */
class FakeAgent : public jsvm::InspectorAgent {
public:
    explicit FakeAgent(JSVM_Env env)
    {
        LogError();
    }
    ~FakeAgent() = default;

public:
    bool Start(const std::string& path, const std::string& hostName, int port, int pid = -1) override
    {
        LogError();
        return false;
    }

    bool Start(const std::string& path, int pid) override
    {
        LogError();
        return false;
    }

    void Stop() override
    {
        LogError();
    }

    bool IsActive() override
    {
        LogError();
        return false;
    }

    void WaitForConnect() override
    {
        LogError();
    }

    void WaitForDisconnect() override
    {
        LogError();
    }

    void PauseOnNextJavascriptStatement(const std::string& reason) override
    {
        LogError();
    }

private:
    void LogError()
    {
        LOG(Error) << "JSVM Inspector is not enabled";
    }
};

} // namespace

jsvm::InspectorAgent* jsvm::InspectorAgent::New(JSVM_Env env)
{
    return new FakeAgent(env);
}
#endif