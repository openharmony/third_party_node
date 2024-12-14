#ifndef SRC_JSVM_HOST_PORT_H_
#define SRC_JSVM_HOST_PORT_H_

#include "jsvm_dfx.h"
#include "jsvm_mutex.h"
#include "jsvm_util.h"

namespace jsvm {

class HostPort {
public:
    HostPort(const std::string& hostName, int port, int pid = -1) : hostName(hostName), port(port), pid(pid) {}
    HostPort(const HostPort&) = default;
    HostPort& operator=(const HostPort&) = default;
    HostPort(HostPort&&) = default;
    HostPort& operator=(HostPort&&) = default;

    void SetHost(const std::string& host)
    {
        hostName = host;
    }

    void SetPort(int portParam)
    {
        port = portParam;
    }

    const std::string& GetHost() const
    {
        return hostName;
    }

    int GetPort() const
    {
        // TODO(joyeecheung): make port a uint16_t
        CHECK_GE(port, 0);
        return port;
    }

    int GetPid() const
    {
        return pid;
    }

    void Update(const HostPort& other)
    {
        if (!other.hostName.empty()) {
            hostName = other.hostName;
        }
        if (other.port >= 0) {
            port = other.port;
        }
    }

private:
    std::string hostName;
    int port;
    int pid;
};

struct InspectPublishUid {
    bool console;
    bool http;
};

} // namespace jsvm

#endif // SRC_JSVM_HOST_PORT_H_
