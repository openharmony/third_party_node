/* Interface for JSVM inspector */

#ifndef JSVM_INSPECTOR_AGENT_H
#define JSVM_INSPECTOR_AGENT_H
#include <string>

#include "jsvm_types.h"

namespace jsvm {
class InspectorAgent {
public:
    static InspectorAgent* New(JSVM_Env);
    virtual ~InspectorAgent() = default;

public:
    virtual bool Start(const std::string& path, const std::string& hostName, int port, int pid = -1) = 0;

    // Find avaliable port and open inspector.
    virtual bool Start(const std::string& path, int pid) = 0;

    virtual void Stop() = 0;

    virtual bool IsActive() = 0;

    virtual void WaitForConnect() = 0;

    virtual void WaitForDisconnect() = 0;

    virtual void PauseOnNextJavascriptStatement(const std::string& reason) = 0;
};
} // namespace jsvm

#endif