#ifndef SRC_INSPECTOR_SOCKET_SERVER_H_
#define SRC_INSPECTOR_SOCKET_SERVER_H_

#include <map>
#include <string>
#include <vector>

#include "inspector_socket.h"
#include "jsvm_host_port.h"
#include "uv.h"

#ifndef ENABLE_INSPECTOR
#error("This header can only be used when inspector is enabled")
#endif

namespace jsvm {
namespace inspector {

std::string FormatWsAddress(const std::string& host, int port, const std::string& targetId, bool includeProtocol);

class InspectorSocketServer;
class SocketSession;
class ServerSocket;

class SocketServerDelegate {
public:
    virtual void AssignServer(InspectorSocketServer* server) = 0;
    virtual void StartSession(int sessionId, const std::string& targetId) = 0;
    virtual void EndSession(int sessionId) = 0;
    virtual void MessageReceived(int sessionId, const std::string& message) = 0;
    virtual std::vector<std::string> GetTargetIds() = 0;
    virtual std::string GetTargetTitle(const std::string& id) = 0;
    virtual std::string GetTargetUrl(const std::string& id) = 0;
    virtual ~SocketServerDelegate() = default;
};

// HTTP Server, writes messages requested as TransportActions, and responds
// to HTTP requests and WS upgrades.

class InspectorSocketServer {
public:
    InspectorSocketServer(std::unique_ptr<SocketServerDelegate> delegateParam,
                          uv_loop_t* loop,
                          const std::string& host,
                          int port,
                          const InspectPublishUid& inspectPublishUid,
                          FILE* out = stderr,
                          int pid = -1);
    ~InspectorSocketServer();

    // Start listening on host/port
    bool Start();

    // Called by the TransportAction sent with InspectorIo::Write():
    //   kKill and kStop
    void Stop();
    //   kSendMessage
    void Send(int sessionId, const std::string& message);
    //   kKill
    void TerminateConnections();
    int GetPort() const;

    // Session connection lifecycle
    void Accept(int serverPort, uv_stream_t* serverSocket);
    bool HandleGetRequest(int sessionId, const std::string& host, const std::string& path);
    void SessionStarted(int sessionId, const std::string& targetId, const std::string& wsId);
    void SessionTerminated(int sessionId);
    void MessageReceived(int sessionId, const std::string& message)
    {
        delegate->MessageReceived(sessionId, message);
    }
    SocketSession* Session(int sessionId);
    bool done() const
    {
        return serverSockets.empty() && connectedSessions.empty();
    }

    static void CloseServerSocket(ServerSocket*);
    using ServerSocketPtr = DeleteFnPtr<ServerSocket, CloseServerSocket>;

private:
    void SendListResponse(InspectorSocket* socket, const std::string& host, SocketSession* session);
    std::string GetFrontendURL(bool is_compat, const std::string& formattedAddress);
    bool TargetExists(const std::string& id);

    enum class ServerState { kNew, kRunning, kStopped };
    uv_loop_t* loop;
    std::unique_ptr<SocketServerDelegate> delegate;
    const std::string host;
    int port;
    InspectPublishUid inspectPublishUid;
    std::vector<ServerSocketPtr> serverSockets;
    std::map<int, std::pair<std::string, std::unique_ptr<SocketSession>>> connectedSessions;
    int nextSessionId;
    FILE* out;
    ServerState state;
    int pid;
};

} // namespace inspector
} // namespace jsvm

#endif // SRC_INSPECTOR_SOCKET_SERVER_H_
