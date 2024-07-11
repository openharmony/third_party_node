/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "js_native_api_v8_inspector.h"

#include "js_native_api_v8.h"
#include "inspector_socket_server.h"
#include "inspector/main_thread_interface.h"
#include "inspector/node_string.h"
#include "inspector/runtime_agent.h"
#include "inspector/tracing_agent.h"
#include "inspector/worker_agent.h"
#include "inspector/worker_inspector.h"
#include "crypto/crypto_util.h"
#include "node/inspector/protocol/Protocol.h"
#include "node_errors.h"
#include "node_internals.h"
#include "node_mutex.h"
#include "node_options-inl.h"
#include "node_process-inl.h"
#include "node_url.h"
#include "permission/permission.h"
#include "util-inl.h"
#include "v8-inspector.h"
#include "v8-platform.h"

#include "libplatform/libplatform.h"

#ifdef __POSIX__
#include <pthread.h>
#include <climits>  // PTHREAD_STACK_MIN
#endif  // __POSIX__

#include <algorithm>
#include <cstring>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace v8impl {

namespace {
using node::ConditionVariable;
using node::Mutex;
using node::inspector::Utf8ToStringView;
using v8_inspector::StringBuffer;
using v8_inspector::StringView;

class MainThreadInterface;

class Request {
 public:
  virtual void Call(MainThreadInterface*) = 0;
  virtual ~Request() = default;
};

class Deletable {
 public:
  virtual ~Deletable() = default;
};

using MessageQueue = std::deque<std::unique_ptr<Request>>;

class MainThreadHandle : public std::enable_shared_from_this<MainThreadHandle> {
 public:
  explicit MainThreadHandle(MainThreadInterface* main_thread)
                            : main_thread_(main_thread) {
  }
  ~MainThreadHandle() {
    Mutex::ScopedLock scoped_lock(block_lock_);
    CHECK_NULL(main_thread_);  // main_thread_ should have called Reset
  }
  std::unique_ptr<InspectorSession> Connect(
      std::unique_ptr<InspectorSessionDelegate> delegate,
      bool prevent_shutdown);
  int newObjectId() {
    return ++next_object_id_;
  }
  bool Post(std::unique_ptr<Request> request);

 private:
  void Reset();

  MainThreadInterface* main_thread_;
  Mutex block_lock_;
  int next_session_id_ = 0;
  std::atomic_int next_object_id_ = {1};

  friend class MainThreadInterface;
};

class MainThreadInterface :
    public std::enable_shared_from_this<MainThreadInterface> {
 public:
  explicit MainThreadInterface(Agent* agent);
  ~MainThreadInterface();

  void DispatchMessages();
  void Post(std::unique_ptr<Request> request);
  bool WaitForFrontendEvent();
  std::shared_ptr<MainThreadHandle> GetHandle();
  Agent* inspector_agent() {
    return agent_;
  }
  void AddObject(int handle, std::unique_ptr<Deletable> object);
  Deletable* GetObject(int id);
  Deletable* GetObjectIfExists(int id);
  void RemoveObject(int handle);

 private:
  MessageQueue requests_;
  Mutex requests_lock_;   // requests_ live across threads
  // This queue is to maintain the order of the messages for the cases
  // when we reenter the DispatchMessages function.
  MessageQueue dispatching_message_queue_;
  bool dispatching_messages_ = false;
  ConditionVariable incoming_message_cond_;
  // Used from any thread
  Agent* const agent_;
  std::shared_ptr<MainThreadHandle> handle_;
  std::unordered_map<int, std::unique_ptr<Deletable>> managed_objects_;
};

template <typename T>
class DeletableWrapper : public Deletable {
 public:
  explicit DeletableWrapper(std::unique_ptr<T> object)
                        : object_(std::move(object)) {}
  ~DeletableWrapper() override = default;

  static T* get(MainThreadInterface* thread, int id) {
    return
        static_cast<DeletableWrapper<T>*>(thread->GetObject(id))->object_.get();
  }

 private:
  std::unique_ptr<T> object_;
};

template <typename T>
std::unique_ptr<Deletable> WrapInDeletable(std::unique_ptr<T> object) {
  return std::unique_ptr<DeletableWrapper<T>>(
      new DeletableWrapper<T>(std::move(object)));
}

template <typename Factory>
class CreateObjectRequest : public Request {
 public:
  CreateObjectRequest(int object_id, Factory factory)
                      : object_id_(object_id), factory_(std::move(factory)) {}

  void Call(MainThreadInterface* thread) override {
    thread->AddObject(object_id_, WrapInDeletable(factory_(thread)));
  }

 private:
  int object_id_;
  Factory factory_;
};

template <typename Factory>
std::unique_ptr<Request> NewCreateRequest(int object_id, Factory factory) {
  return std::unique_ptr<Request>(
      new CreateObjectRequest<Factory>(object_id, std::move(factory)));
}

class DeleteRequest : public Request {
 public:
  explicit DeleteRequest(int object_id) : object_id_(object_id) {}

  void Call(MainThreadInterface* thread) override {
    thread->RemoveObject(object_id_);
  }

 private:
  int object_id_;
};

template <typename Target, typename Fn>
class CallRequest : public Request {
 public:
  CallRequest(int id, Fn fn) : id_(id), fn_(std::move(fn)) {}

  void Call(MainThreadInterface* thread) override {
    fn_(DeletableWrapper<Target>::get(thread, id_));
  }

 private:
  int id_;
  Fn fn_;
};

template <typename T>
class AnotherThreadObjectReference {
 public:
  AnotherThreadObjectReference(
      std::shared_ptr<MainThreadHandle> thread, int object_id)
      : thread_(thread), object_id_(object_id) {}

  template <typename Factory>
  AnotherThreadObjectReference(
      std::shared_ptr<MainThreadHandle> thread, Factory factory)
      : AnotherThreadObjectReference(thread, thread->newObjectId()) {
    thread_->Post(NewCreateRequest(object_id_, std::move(factory)));
  }
  AnotherThreadObjectReference(AnotherThreadObjectReference&) = delete;

  ~AnotherThreadObjectReference() {
    // Disappearing thread may cause a memory leak
    thread_->Post(std::make_unique<DeleteRequest>(object_id_));
  }

  template <typename Fn>
  void Call(Fn fn) const {
    using Request = CallRequest<T, Fn>;
    thread_->Post(std::unique_ptr<Request>(
        new Request(object_id_, std::move(fn))));
  }

  template <typename Arg>
  void Call(void (T::*fn)(Arg), Arg argument) const {
    Call(std::bind(Apply<Arg>, std::placeholders::_1, fn, std::move(argument)));
  }

 private:
  // This has to use non-const reference to support std::bind with non-copyable
  // types
  template <typename Argument>
  static void Apply(T* target, void (T::*fn)(Argument),
    /* NOLINT (runtime/references) */ Argument& argument) {
    (target->*fn)(std::move(argument));
  }

  std::shared_ptr<MainThreadHandle> thread_;
  const int object_id_;
};

class MainThreadSessionState {
 public:
  MainThreadSessionState(MainThreadInterface* thread, bool prevent_shutdown)
                         : thread_(thread),
                           prevent_shutdown_(prevent_shutdown) {}

  static std::unique_ptr<MainThreadSessionState> Create(
      MainThreadInterface* thread, bool prevent_shutdown) {
    return std::make_unique<MainThreadSessionState>(thread, prevent_shutdown);
  }

  void Connect(std::unique_ptr<InspectorSessionDelegate> delegate) {
    Agent* agent = thread_->inspector_agent();
    if (agent != nullptr)
      session_ = agent->Connect(std::move(delegate), prevent_shutdown_);
  }

  void Dispatch(std::unique_ptr<StringBuffer> message) {
    session_->Dispatch(message->string());
  }

 private:
  MainThreadInterface* thread_;
  bool prevent_shutdown_;
  std::unique_ptr<InspectorSession> session_;
};

class CrossThreadInspectorSession : public InspectorSession {
 public:
  CrossThreadInspectorSession(
      int id,
      std::shared_ptr<MainThreadHandle> thread,
      std::unique_ptr<InspectorSessionDelegate> delegate,
      bool prevent_shutdown)
      : state_(thread, std::bind(MainThreadSessionState::Create,
                                 std::placeholders::_1,
                                 prevent_shutdown)) {
    state_.Call(&MainThreadSessionState::Connect, std::move(delegate));
  }

  void Dispatch(const StringView& message) override {
    state_.Call(&MainThreadSessionState::Dispatch,
                StringBuffer::create(message));
  }

 private:
  AnotherThreadObjectReference<MainThreadSessionState> state_;
};

class ThreadSafeDelegate : public InspectorSessionDelegate {
 public:
  ThreadSafeDelegate(std::shared_ptr<MainThreadHandle> thread, int object_id)
                     : thread_(thread), delegate_(thread, object_id) {}

  void SendMessageToFrontend(const v8_inspector::StringView& message) override {
    delegate_.Call(
        [m = StringBuffer::create(message)]
        (InspectorSessionDelegate* delegate) {
      delegate->SendMessageToFrontend(m->string());
    });
  }

 private:
  std::shared_ptr<MainThreadHandle> thread_;
  AnotherThreadObjectReference<InspectorSessionDelegate> delegate_;
};

MainThreadInterface::MainThreadInterface(Agent* agent) : agent_(agent) {}

MainThreadInterface::~MainThreadInterface() {
  if (handle_)
    handle_->Reset();
}

void MainThreadInterface::Post(std::unique_ptr<Request> request) {
  CHECK_NOT_NULL(agent_);
  Mutex::ScopedLock scoped_lock(requests_lock_);
  bool needs_notify = requests_.empty();
  requests_.push_back(std::move(request));
  if (needs_notify) {
    std::weak_ptr<MainThreadInterface> weak_self {shared_from_this()};
    agent_->env()->RequestInterrupt([weak_self](Environment*) {
      if (auto iface = weak_self.lock()) iface->DispatchMessages();
    });
  }
  incoming_message_cond_.Broadcast(scoped_lock);
}

bool MainThreadInterface::WaitForFrontendEvent() {
  // We allow DispatchMessages reentry as we enter the pause. This is important
  // to support debugging the code invoked by an inspector call, such
  // as Runtime.evaluate
  dispatching_messages_ = false;
  if (dispatching_message_queue_.empty()) {
    Mutex::ScopedLock scoped_lock(requests_lock_);
    while (requests_.empty()) incoming_message_cond_.Wait(scoped_lock);
  }
  return true;
}

void MainThreadInterface::DispatchMessages() {
  if (dispatching_messages_)
    return;
  dispatching_messages_ = true;
  bool had_messages = false;
  do {
    if (dispatching_message_queue_.empty()) {
      Mutex::ScopedLock scoped_lock(requests_lock_);
      requests_.swap(dispatching_message_queue_);
    }
    had_messages = !dispatching_message_queue_.empty();
    while (!dispatching_message_queue_.empty()) {
      MessageQueue::value_type task;
      std::swap(dispatching_message_queue_.front(), task);
      dispatching_message_queue_.pop_front();

      v8::SealHandleScope seal_handle_scope(agent_->env()->isolate);
      task->Call(this);
    }
  } while (had_messages);
  dispatching_messages_ = false;
}

std::shared_ptr<MainThreadHandle> MainThreadInterface::GetHandle() {
  if (handle_ == nullptr)
    handle_ = std::make_shared<MainThreadHandle>(this);
  return handle_;
}

void MainThreadInterface::AddObject(int id,
                                    std::unique_ptr<Deletable> object) {
  CHECK_NOT_NULL(object);
  managed_objects_[id] = std::move(object);
}

void MainThreadInterface::RemoveObject(int id) {
  CHECK_EQ(1, managed_objects_.erase(id));
}

Deletable* MainThreadInterface::GetObject(int id) {
  Deletable* pointer = GetObjectIfExists(id);
  // This would mean the object is requested after it was disposed, which is
  // a coding error.
  CHECK_NOT_NULL(pointer);
  return pointer;
}

Deletable* MainThreadInterface::GetObjectIfExists(int id) {
  auto iterator = managed_objects_.find(id);
  if (iterator == managed_objects_.end()) {
    return nullptr;
  }
  return iterator->second.get();
}

std::unique_ptr<InspectorSession> MainThreadHandle::Connect(
    std::unique_ptr<InspectorSessionDelegate> delegate,
    bool prevent_shutdown) {
  return std::unique_ptr<InspectorSession>(
      new CrossThreadInspectorSession(++next_session_id_,
                                      shared_from_this(),
                                      std::move(delegate),
                                      prevent_shutdown));
}

bool MainThreadHandle::Post(std::unique_ptr<Request> request) {
  Mutex::ScopedLock scoped_lock(block_lock_);
  if (!main_thread_)
    return false;
  main_thread_->Post(std::move(request));
  return true;
}

void MainThreadHandle::Reset() {
  Mutex::ScopedLock scoped_lock(block_lock_);
  main_thread_ = nullptr;
}
}  // namespace

namespace {
using node::CheckedUvLoopClose;
using node::GetHumanReadableProcessName;
using node::InspectPublishUid;
using node::RegisterSignalHandler;
using node::inspector::FormatWsAddress;
using node::inspector::InspectorSocketServer;

namespace crypto = node::crypto;
namespace protocol = node::inspector::protocol;

// kKill closes connections and stops the server, kStop only stops the server
enum class TransportAction { kKill, kSendMessage, kStop };

std::string ScriptPath(uv_loop_t* loop, const std::string& script_name) {
  std::string script_path;

  if (!script_name.empty()) {
    uv_fs_t req;
    req.ptr = nullptr;
    if (0 == uv_fs_realpath(loop, &req, script_name.c_str(), nullptr)) {
      CHECK_NOT_NULL(req.ptr);
      script_path = std::string(static_cast<char*>(req.ptr));
    }
    uv_fs_req_cleanup(&req);
  }

  return script_path;
}

// UUID RFC: https://www.ietf.org/rfc/rfc4122.txt
// Used ver 4 - with numbers
std::string GenerateID() {
  uint16_t buffer[8];
  CHECK(crypto::CSPRNG(buffer, sizeof(buffer)).is_ok());

  char uuid[256];
  snprintf(uuid, sizeof(uuid), "%04x%04x-%04x-%04x-%04x-%04x%04x%04x",
           buffer[0],  // time_low
           buffer[1],  // time_mid
           buffer[2],  // time_low
           (buffer[3] & 0x0fff) | 0x4000,  // time_hi_and_version
           (buffer[4] & 0x3fff) | 0x8000,  // clk_seq_hi clk_seq_low
           buffer[5],  // node
           buffer[6],
           buffer[7]);
  return uuid;
}

class RequestToServer {
 public:
  RequestToServer(TransportAction action,
                  int session_id,
                  std::unique_ptr<v8_inspector::StringBuffer> message)
                  : action_(action),
                    session_id_(session_id),
                    message_(std::move(message)) {}

  void Dispatch(InspectorSocketServer* server) const {
    switch (action_) {
      case TransportAction::kKill:
        server->TerminateConnections();
        [[fallthrough]];
      case TransportAction::kStop:
        server->Stop();
        break;
      case TransportAction::kSendMessage:
        server->Send(
            session_id_,
            protocol::StringUtil::StringViewToUtf8(message_->string()));
        break;
    }
  }

 private:
  TransportAction action_;
  int session_id_;
  std::unique_ptr<v8_inspector::StringBuffer> message_;
};

class RequestQueue;

class RequestQueueData {
 public:
  using MessageQueue = std::deque<RequestToServer>;

  explicit RequestQueueData(uv_loop_t* loop)
                            : handle_(std::make_shared<RequestQueue>(this)) {
    int err = uv_async_init(loop, &async_, [](uv_async_t* async) {
      RequestQueueData* wrapper =
          node::ContainerOf(&RequestQueueData::async_, async);
      wrapper->DoDispatch();
    });
    CHECK_EQ(0, err);
  }

  static void CloseAndFree(RequestQueueData* queue);

  void Post(int session_id,
            TransportAction action,
            std::unique_ptr<StringBuffer> message) {
    Mutex::ScopedLock scoped_lock(state_lock_);
    bool notify = messages_.empty();
    messages_.emplace_back(action, session_id, std::move(message));
    if (notify) {
      CHECK_EQ(0, uv_async_send(&async_));
      incoming_message_cond_.Broadcast(scoped_lock);
    }
  }

  void Wait() {
    Mutex::ScopedLock scoped_lock(state_lock_);
    if (messages_.empty()) {
      incoming_message_cond_.Wait(scoped_lock);
    }
  }

  void SetServer(InspectorSocketServer* server) {
    server_ = server;
  }

  std::shared_ptr<RequestQueue> handle() {
    return handle_;
  }

 private:
  ~RequestQueueData() = default;

  MessageQueue GetMessages() {
    Mutex::ScopedLock scoped_lock(state_lock_);
    MessageQueue messages;
    messages_.swap(messages);
    return messages;
  }

  void DoDispatch() {
    if (server_ == nullptr)
      return;
    for (const auto& request : GetMessages()) {
      request.Dispatch(server_);
    }
  }

  std::shared_ptr<RequestQueue> handle_;
  uv_async_t async_;
  InspectorSocketServer* server_ = nullptr;
  MessageQueue messages_;
  Mutex state_lock_;  // Locked before mutating the queue.
  ConditionVariable incoming_message_cond_;
};

class RequestQueue {
 public:
  explicit RequestQueue(RequestQueueData* data) : data_(data) {}

  void Reset() {
    Mutex::ScopedLock scoped_lock(lock_);
    data_ = nullptr;
  }

  void Post(int session_id,
            TransportAction action,
            std::unique_ptr<StringBuffer> message) {
    Mutex::ScopedLock scoped_lock(lock_);
    if (data_ != nullptr)
      data_->Post(session_id, action, std::move(message));
  }

  bool Expired() {
    Mutex::ScopedLock scoped_lock(lock_);
    return data_ == nullptr;
  }

 private:
  RequestQueueData* data_;
  Mutex lock_;
};

class IoSessionDelegate : public InspectorSessionDelegate {
 public:
  explicit IoSessionDelegate(std::shared_ptr<RequestQueue> queue, int id)
                             : request_queue_(queue), id_(id) { }
  void SendMessageToFrontend(const v8_inspector::StringView& message) override {
    request_queue_->Post(id_, TransportAction::kSendMessage,
                         StringBuffer::create(message));
  }

 private:
  std::shared_ptr<RequestQueue> request_queue_;
  int id_;
};

// Passed to InspectorSocketServer to handle WS inspector protocol events,
// mostly session start, message received, and session end.
class InspectorIoDelegate: public node::inspector::SocketServerDelegate {
 public:
  InspectorIoDelegate(std::shared_ptr<RequestQueueData> queue,
                      std::shared_ptr<MainThreadHandle> main_thread,
                      const std::string& target_id,
                      const std::string& script_path,
                      const std::string& script_name);
  ~InspectorIoDelegate() override = default;

  void StartSession(int session_id, const std::string& target_id) override;
  void MessageReceived(int session_id, const std::string& message) override;
  void EndSession(int session_id) override;

  std::vector<std::string> GetTargetIds() override;
  std::string GetTargetTitle(const std::string& id) override;
  std::string GetTargetUrl(const std::string& id) override;
  void AssignServer(InspectorSocketServer* server) override {
    request_queue_->SetServer(server);
  }

 private:
  std::shared_ptr<RequestQueueData> request_queue_;
  std::shared_ptr<MainThreadHandle> main_thread_;
  std::unordered_map<int, std::unique_ptr<InspectorSession>> sessions_;
  const std::string script_name_;
  const std::string script_path_;
  const std::string target_id_;
};

InspectorIoDelegate::InspectorIoDelegate(
    std::shared_ptr<RequestQueueData> queue,
    std::shared_ptr<MainThreadHandle> main_thread,
    const std::string& target_id,
    const std::string& script_path,
    const std::string& script_name)
    : request_queue_(queue), main_thread_(main_thread),
      script_name_(script_name), script_path_(script_path),
      target_id_(target_id) {}

void InspectorIoDelegate::StartSession(int session_id,
                                       const std::string& target_id) {
  auto session = main_thread_->Connect(
      std::unique_ptr<InspectorSessionDelegate>(
          new IoSessionDelegate(request_queue_->handle(), session_id)), true);
  if (session) {
    sessions_[session_id] = std::move(session);
    fprintf(stderr, "Debugger attached.\n");
  }
}

void InspectorIoDelegate::MessageReceived(int session_id,
                                          const std::string& message) {
  auto session = sessions_.find(session_id);
  if (session != sessions_.end())
    session->second->Dispatch(Utf8ToStringView(message)->string());
}

void InspectorIoDelegate::EndSession(int session_id) {
  sessions_.erase(session_id);
}

std::vector<std::string> InspectorIoDelegate::GetTargetIds() {
  return { target_id_ };
}

std::string InspectorIoDelegate::GetTargetTitle(const std::string& id) {
  return script_name_.empty() ? GetHumanReadableProcessName() : script_name_;
}

std::string InspectorIoDelegate::GetTargetUrl(const std::string& id) {
  return "file://" + script_path_;
}

// static
void RequestQueueData::CloseAndFree(RequestQueueData* queue) {
  queue->handle_->Reset();
  queue->handle_.reset();
  uv_close(reinterpret_cast<uv_handle_t*>(&queue->async_),
           [](uv_handle_t* handle) {
    uv_async_t* async = reinterpret_cast<uv_async_t*>(handle);
    RequestQueueData* wrapper =
        node::ContainerOf(&RequestQueueData::async_, async);
    delete wrapper;
  });
}
}  // namespace

class InspectorIo {
 public:
  // Start the inspector agent thread, waiting for it to initialize
  // bool Start();
  // Returns empty pointer if thread was not started
  static std::unique_ptr<InspectorIo> Start(
      std::shared_ptr<MainThreadHandle> main_thread,
      const std::string& path,
      std::shared_ptr<ExclusiveAccess<HostPort>> host_port,
      const node::InspectPublishUid& inspect_publish_uid);

  // Will block till the transport thread shuts down
  ~InspectorIo();

  void StopAcceptingNewConnections();
  std::string GetWsUrl() const;

 private:
  InspectorIo(std::shared_ptr<MainThreadHandle> handle,
              const std::string& path,
              std::shared_ptr<ExclusiveAccess<HostPort>> host_port,
              const node::InspectPublishUid& inspect_publish_uid);

  // Wrapper for agent->ThreadMain()
  static void ThreadMain(void* agent);

  // Runs a uv_loop_t
  void ThreadMain();

  // This is a thread-safe object that will post async tasks. It lives as long
  // as an Inspector object lives (almost as long as an Isolate).
  std::shared_ptr<MainThreadHandle> main_thread_;
  // Used to post on a frontend interface thread, lives while the server is
  // running
  std::shared_ptr<RequestQueue> request_queue_;
  std::shared_ptr<ExclusiveAccess<HostPort>> host_port_;
  node::InspectPublishUid inspect_publish_uid_;

  // The IO thread runs its own uv_loop to implement the TCP server off
  // the main thread.
  uv_thread_t thread_;

  // For setting up interthread communications
  Mutex thread_start_lock_;
  node::ConditionVariable thread_start_condition_;
  std::string script_name_;
  // May be accessed from any thread
  const std::string id_;
};

// static
std::unique_ptr<InspectorIo> InspectorIo::Start(
    std::shared_ptr<MainThreadHandle> main_thread,
    const std::string& path,
    std::shared_ptr<ExclusiveAccess<HostPort>> host_port,
    const InspectPublishUid& inspect_publish_uid) {
  auto io = std::unique_ptr<InspectorIo>(
      new InspectorIo(main_thread,
                      path,
                      host_port,
                      inspect_publish_uid));
  if (io->request_queue_->Expired()) {  // Thread is not running
    return nullptr;
  }
  return io;
}

InspectorIo::InspectorIo(std::shared_ptr<MainThreadHandle> main_thread,
                         const std::string& path,
                         std::shared_ptr<ExclusiveAccess<HostPort>> host_port,
                         const InspectPublishUid& inspect_publish_uid)
    : main_thread_(main_thread),
      host_port_(host_port),
      inspect_publish_uid_(inspect_publish_uid),
      thread_(),
      script_name_(path),
      id_(GenerateID()) {
  Mutex::ScopedLock scoped_lock(thread_start_lock_);
  CHECK_EQ(uv_thread_create(&thread_, InspectorIo::ThreadMain, this), 0);
  thread_start_condition_.Wait(scoped_lock);
}

InspectorIo::~InspectorIo() {
  request_queue_->Post(0, TransportAction::kKill, nullptr);
  int err = uv_thread_join(&thread_);
  CHECK_EQ(err, 0);
}

void InspectorIo::StopAcceptingNewConnections() {
  request_queue_->Post(0, TransportAction::kStop, nullptr);
}

// static
void InspectorIo::ThreadMain(void* io) {
  static_cast<InspectorIo*>(io)->ThreadMain();
}

void InspectorIo::ThreadMain() {
  uv_loop_t loop;
  loop.data = nullptr;
  int err = uv_loop_init(&loop);
  CHECK_EQ(err, 0);
  std::shared_ptr<RequestQueueData> queue(new RequestQueueData(&loop),
                                          RequestQueueData::CloseAndFree);
  std::string script_path = ScriptPath(&loop, script_name_);
  std::unique_ptr<InspectorIoDelegate> delegate(
      new InspectorIoDelegate(queue, main_thread_, id_,
                              script_path, script_name_));
  std::string host;
  int port;
  {
    ExclusiveAccess<HostPort>::Scoped host_port(host_port_);
    host = host_port->host();
    port = host_port->port();
  }
  InspectorSocketServer server(std::move(delegate),
                               &loop,
                               std::move(host),
                               port,
                               inspect_publish_uid_);
  request_queue_ = queue->handle();
  // Its lifetime is now that of the server delegate
  queue.reset();
  {
    Mutex::ScopedLock scoped_lock(thread_start_lock_);
    if (server.Start()) {
      ExclusiveAccess<HostPort>::Scoped host_port(host_port_);
      host_port->set_port(server.Port());
    }
    thread_start_condition_.Broadcast(scoped_lock);
  }
  uv_run(&loop, UV_RUN_DEFAULT);
  CheckedUvLoopClose(&loop);
}

std::string InspectorIo::GetWsUrl() const {
  ExclusiveAccess<HostPort>::Scoped host_port(host_port_);
  return FormatWsAddress(host_port->host(), host_port->port(), id_, true);
}

namespace {

using node::DebugCategory;
using node::TwoByteValue;

using v8::Context;
using v8::Function;
using v8::HandleScope;
using v8::Isolate;
using v8::Local;
using v8::Message;
using v8::Object;
using v8::Value;

using v8_inspector::StringBuffer;
using v8_inspector::StringView;
using v8_inspector::V8Inspector;
using v8_inspector::V8InspectorClient;

namespace per_process = node::per_process;

std::unique_ptr<StringBuffer> ToProtocolString(Isolate* isolate,
                                               Local<Value> value) {
  TwoByteValue buffer(isolate, value);
  return StringBuffer::create(StringView(*buffer, buffer.length()));
}


const int CONTEXT_GROUP_ID = 1;

std::string GetWorkerLabel(Environment* env) {
  std::ostringstream result;
  // TODO: use thread ID as part of worker label.
  result << "Worker[" << "env->thread_id()" << "]";
  return result.str();
}

class ChannelImpl final : public v8_inspector::V8Inspector::Channel,
                          public protocol::FrontendChannel {
 public:
  explicit ChannelImpl(const std::unique_ptr<V8Inspector>& inspector,
                       std::unique_ptr<InspectorSessionDelegate> delegate,
                       std::shared_ptr<MainThreadHandle> main_thread_,
                       bool prevent_shutdown)
      : delegate_(std::move(delegate)), prevent_shutdown_(prevent_shutdown),
        retaining_context_(false) {
    session_ = inspector->connect(CONTEXT_GROUP_ID,
                                  this,
                                  StringView(),
                                  V8Inspector::ClientTrustLevel::kFullyTrusted);
    node_dispatcher_ = std::make_unique<protocol::UberDispatcher>(this);
    runtime_agent_ = std::make_unique<protocol::RuntimeAgent>();
    runtime_agent_->Wire(node_dispatcher_.get());
  }

  ~ChannelImpl() override {
    if (worker_agent_) {
      worker_agent_->disable();
      worker_agent_.reset();  // Dispose before the dispatchers
    }
    runtime_agent_->disable();
    runtime_agent_.reset();  // Dispose before the dispatchers
  }

  void dispatchProtocolMessage(const StringView& message) {
    std::string raw_message = protocol::StringUtil::StringViewToUtf8(message);
    per_process::Debug(DebugCategory::INSPECTOR_SERVER,
                       "[inspector received] %s\n",
                       raw_message);
    std::unique_ptr<protocol::DictionaryValue> value =
        protocol::DictionaryValue::cast(
            protocol::StringUtil::parseJSON(message));
    int call_id;
    std::string method;
    node_dispatcher_->parseCommand(value.get(), &call_id, &method);
    if (v8_inspector::V8InspectorSession::canDispatchMethod(
            Utf8ToStringView(method)->string())) {
      session_->dispatchProtocolMessage(message);
    } else {
      node_dispatcher_->dispatch(call_id, method, std::move(value),
                                 raw_message);
    }
  }

  void schedulePauseOnNextStatement(const std::string& reason) {
    std::unique_ptr<StringBuffer> buffer = Utf8ToStringView(reason);
    session_->schedulePauseOnNextStatement(buffer->string(), buffer->string());
  }

  bool preventShutdown() {
    return prevent_shutdown_;
  }

  bool notifyWaitingForDisconnect() {
    retaining_context_ = runtime_agent_->notifyWaitingForDisconnect();
    return retaining_context_;
  }

  bool retainingContext() {
    return retaining_context_;
  }

 private:
  void sendResponse(
      int callId,
      std::unique_ptr<v8_inspector::StringBuffer> message) override {
    sendMessageToFrontend(message->string());
  }

  void sendNotification(
      std::unique_ptr<v8_inspector::StringBuffer> message) override {
    sendMessageToFrontend(message->string());
  }

  void flushProtocolNotifications() override { }

  void sendMessageToFrontend(const StringView& message) {
    if (per_process::enabled_debug_list.enabled(
            DebugCategory::INSPECTOR_SERVER)) {
      std::string raw_message = protocol::StringUtil::StringViewToUtf8(message);
      per_process::Debug(DebugCategory::INSPECTOR_SERVER,
                         "[inspector send] %s\n",
                         raw_message);
    }
    delegate_->SendMessageToFrontend(message);
  }

  void sendMessageToFrontend(const std::string& message) {
    sendMessageToFrontend(Utf8ToStringView(message)->string());
  }

  using Serializable = protocol::Serializable;

  void sendProtocolResponse(int callId,
                            std::unique_ptr<Serializable> message) override {
    sendMessageToFrontend(message->serializeToJSON());
  }
  void sendProtocolNotification(
      std::unique_ptr<Serializable> message) override {
    sendMessageToFrontend(message->serializeToJSON());
  }

  void fallThrough(int callId,
                   const std::string& method,
                   const std::string& message) override {
    DCHECK(false);
  }

  std::unique_ptr<protocol::RuntimeAgent> runtime_agent_;
  std::unique_ptr<protocol::WorkerAgent> worker_agent_;
  std::unique_ptr<InspectorSessionDelegate> delegate_;
  std::unique_ptr<v8_inspector::V8InspectorSession> session_;
  std::unique_ptr<protocol::UberDispatcher> node_dispatcher_;
  bool prevent_shutdown_;
  bool retaining_context_;
};

class SameThreadInspectorSession : public InspectorSession {
 public:
  SameThreadInspectorSession(
      int session_id, std::shared_ptr<InspectorClient> client)
      : session_id_(session_id), client_(client) {}
  ~SameThreadInspectorSession() override;
  void Dispatch(const v8_inspector::StringView& message) override;

 private:
  int session_id_;
  std::weak_ptr<InspectorClient> client_;
};

bool IsFilePath(const std::string& path) {
  return !path.empty() && path[0] == '/';
}

void ThrowUninitializedInspectorError(Environment* env) {
  HandleScope scope(env->isolate);

  const char* msg = "This Environment was initialized without a V8::Inspector";
  Local<Value> exception =
    v8::String::NewFromUtf8(env->isolate, msg).ToLocalChecked();

  env->isolate->ThrowException(exception);
}

}  // namespace

class InspectorClient : public V8InspectorClient {
 public:
  explicit InspectorClient(Environment* env, bool is_main)
      : env_(env), is_main_(is_main) {
    client_ = V8Inspector::create(env->isolate, this);
    // TODO(bnoordhuis) Make name configurable from src/node.cc.
    std::string name =
        is_main_ ? GetHumanReadableProcessName() : GetWorkerLabel(env);
    ContextInfo info(name);
    info.is_default = true;
    contextCreated(env->context(), info);
  }

  void runMessageLoopOnPause(int context_group_id) override {
    waiting_for_resume_ = true;
    runMessageLoop();
  }

  void waitForSessionsDisconnect() {
    waiting_for_sessions_disconnect_ = true;
    runMessageLoop();
  }

  void waitForFrontend() {
    waiting_for_frontend_ = true;
    runMessageLoop();
  }

  void maxAsyncCallStackDepthChanged(int depth) override {
    if (waiting_for_sessions_disconnect_) {
      // V8 isolate is mostly done and is only letting Inspector protocol
      // clients gather data.
      return;
    }
  }

  void contextCreated(Local<Context> context, const ContextInfo& info) {
    auto name_buffer = Utf8ToStringView(info.name);
    auto origin_buffer = Utf8ToStringView(info.origin);
    std::unique_ptr<StringBuffer> aux_data_buffer;

    v8_inspector::V8ContextInfo v8info(
        context, CONTEXT_GROUP_ID, name_buffer->string());
    v8info.origin = origin_buffer->string();

    if (info.is_default) {
      aux_data_buffer = Utf8ToStringView("{\"isDefault\":true}");
    } else {
      aux_data_buffer = Utf8ToStringView("{\"isDefault\":false}");
    }
    v8info.auxData = aux_data_buffer->string();

    client_->contextCreated(v8info);
  }

  void contextDestroyed(Local<Context> context) {
    client_->contextDestroyed(context);
  }

  void quitMessageLoopOnPause() override {
    waiting_for_resume_ = false;
  }

  void runIfWaitingForDebugger(int context_group_id) override {
    waiting_for_frontend_ = false;
  }

  int connectFrontend(std::unique_ptr<InspectorSessionDelegate> delegate,
                      bool prevent_shutdown) {
    int session_id = next_session_id_++;
    channels_[session_id] = std::make_unique<ChannelImpl>(client_,
                                                          std::move(delegate),
                                                          getThreadHandle(),
                                                          prevent_shutdown);
    return session_id;
  }

  void disconnectFrontend(int session_id) {
    auto it = channels_.find(session_id);
    if (it == channels_.end())
      return;
    bool retaining_context = it->second->retainingContext();
    channels_.erase(it);
    if (retaining_context) {
      for (const auto& id_channel : channels_) {
        if (id_channel.second->retainingContext())
          return;
      }
      contextDestroyed(env_->context());
    }
    if (waiting_for_sessions_disconnect_ && !is_main_)
      waiting_for_sessions_disconnect_ = false;
  }

  void dispatchMessageFromFrontend(int session_id, const StringView& message) {
    channels_[session_id]->dispatchProtocolMessage(message);
  }

  Local<Context> ensureDefaultContextInGroup(int contextGroupId) override {
    return env_->context();
  }

  void ReportUncaughtException(Local<Value> error, Local<Message> message) {
    Isolate* isolate = env_->isolate;
    Local<Context> context = env_->context();

    int script_id = message->GetScriptOrigin().ScriptId();

    Local<v8::StackTrace> stack_trace = message->GetStackTrace();

    if (!stack_trace.IsEmpty() && stack_trace->GetFrameCount() > 0 &&
        script_id == stack_trace->GetFrame(isolate, 0)->GetScriptId()) {
      script_id = 0;
    }

    const uint8_t DETAILS[] = "Uncaught";

    client_->exceptionThrown(
        context,
        StringView(DETAILS, sizeof(DETAILS) - 1),
        error,
        ToProtocolString(isolate, message->Get())->string(),
        ToProtocolString(isolate, message->GetScriptResourceName())->string(),
        message->GetLineNumber(context).FromMaybe(0),
        message->GetStartColumn(context).FromMaybe(0),
        client_->createStackTrace(stack_trace),
        script_id);
  }

  void startRepeatingTimer(double interval_s,
                           TimerCallback callback,
                           void* data) override {
    // TODO: implement this for supporting heap profiler.
  }

  void cancelTimer(void* data) override {
    // TODO: implement this for supporting heap profiler.
  }

  // Async stack traces instrumentation.
  void AsyncTaskScheduled(const StringView& task_name, void* task,
                          bool recurring) {
    client_->asyncTaskScheduled(task_name, task, recurring);
  }

  void AsyncTaskCanceled(void* task) {
    client_->asyncTaskCanceled(task);
  }

  void AsyncTaskStarted(void* task) {
    client_->asyncTaskStarted(task);
  }

  void AsyncTaskFinished(void* task) {
    client_->asyncTaskFinished(task);
  }

  void AllAsyncTasksCanceled() {
    client_->allAsyncTasksCanceled();
  }

  void schedulePauseOnNextStatement(const std::string& reason) {
    for (const auto& id_channel : channels_) {
      id_channel.second->schedulePauseOnNextStatement(reason);
    }
  }

  bool hasConnectedSessions() {
    for (const auto& id_channel : channels_) {
      // Other sessions are "invisible" more most purposes
      if (id_channel.second->preventShutdown())
        return true;
    }
    return false;
  }

  bool notifyWaitingForDisconnect() {
    bool retaining_context = false;
    for (const auto& id_channel : channels_) {
      if (id_channel.second->notifyWaitingForDisconnect())
        retaining_context = true;
    }
    return retaining_context;
  }

  std::shared_ptr<MainThreadHandle> getThreadHandle() {
    if (!interface_) {
      interface_ = std::make_shared<MainThreadInterface>(
          env_->inspector_agent());
    }
    return interface_->GetHandle();
  }

  bool IsActive() {
    return !channels_.empty();
  }

 private:
  bool shouldRunMessageLoop() {
    if (waiting_for_frontend_)
      return true;
    if (waiting_for_sessions_disconnect_ || waiting_for_resume_) {
      return hasConnectedSessions();
    }
    return false;
  }

  void runMessageLoop() {
    if (running_nested_loop_)
      return;

    running_nested_loop_ = true;

    while (shouldRunMessageLoop()) {
      if (interface_) interface_->WaitForFrontendEvent();
      env_->RunAndClearInterrupts();
    }
    running_nested_loop_ = false;
  }

  double currentTimeMS() override {
    return env_->platform()->CurrentClockTimeMillis();
  }

  std::unique_ptr<StringBuffer> resourceNameToUrl(
      const StringView& resource_name_view) override {
    std::string resource_name =
        protocol::StringUtil::StringViewToUtf8(resource_name_view);
    if (!IsFilePath(resource_name))
      return nullptr;

    std::string url = node::url::FromFilePath(resource_name);
    return Utf8ToStringView(url);
  }

  Environment* env_;
  bool is_main_;
  bool running_nested_loop_ = false;
  std::unique_ptr<V8Inspector> client_;
  std::unordered_map<int, std::unique_ptr<ChannelImpl>> channels_;
  int next_session_id_ = 1;
  bool waiting_for_resume_ = false;
  bool waiting_for_frontend_ = false;
  bool waiting_for_sessions_disconnect_ = false;
  // Allows accessing Inspector from non-main threads
  std::shared_ptr<MainThreadInterface> interface_;
};

Agent::Agent(Environment* env)
    : parent_env_(env) {}

Agent::~Agent() = default;

bool Agent::Start(const std::string& path,
                  std::shared_ptr<ExclusiveAccess<HostPort>> host_port,
                  bool is_main,
                  bool wait_for_connect) {
  path_ = path;
  CHECK_NOT_NULL(host_port);
  host_port_ = host_port;

  client_ = std::make_shared<InspectorClient>(parent_env_, is_main);

  if (!StartIoThread()) {
    return false;
  }

  if (wait_for_connect) {
    client_->waitForFrontend();
  }
  return true;
}

bool Agent::StartIoThread() {
  if (io_ != nullptr)
    return true;

  if (!client_) {
    ThrowUninitializedInspectorError(parent_env_);
    return false;
  }

  CHECK_NOT_NULL(client_);

  io_ = InspectorIo::Start(client_->getThreadHandle(),
                           path_,
                           host_port_,
                           { false, true });
  if (io_ == nullptr) {
    return false;
  }
  return true;
}

void Agent::Stop() {
  io_.reset();
}

std::unique_ptr<InspectorSession> Agent::Connect(
    std::unique_ptr<InspectorSessionDelegate> delegate,
    bool prevent_shutdown) {
  if (!client_) {
    ThrowUninitializedInspectorError(parent_env_);
    return std::unique_ptr<InspectorSession>{};
  }

  CHECK_NOT_NULL(client_);

  int session_id = client_->connectFrontend(std::move(delegate),
                                            prevent_shutdown);
  return std::unique_ptr<InspectorSession>(
      new SameThreadInspectorSession(session_id, client_));
}

void Agent::WaitForDisconnect() {
  if (!client_) {
    ThrowUninitializedInspectorError(parent_env_);
    return;
  }

  CHECK_NOT_NULL(client_);
  if (client_->hasConnectedSessions()) {
    fprintf(stderr, "Waiting for the debugger to disconnect...\n");
    fflush(stderr);
  }
  if (!client_->notifyWaitingForDisconnect()) {
    client_->contextDestroyed(parent_env_->context());
  }
  if (io_ != nullptr) {
    io_->StopAcceptingNewConnections();
    client_->waitForSessionsDisconnect();
  }
}

void Agent::ReportUncaughtException(Local<Value> error,
                                    Local<Message> message) {
  if (!IsListening())
    return;
  client_->ReportUncaughtException(error, message);
  WaitForDisconnect();
}

void Agent::PauseOnNextJavascriptStatement(const std::string& reason) {
  client_->schedulePauseOnNextStatement(reason);
}

void Agent::AsyncTaskScheduled(const StringView& task_name, void* task,
                               bool recurring) {
  client_->AsyncTaskScheduled(task_name, task, recurring);
}

void Agent::AsyncTaskCanceled(void* task) {
  client_->AsyncTaskCanceled(task);
}

void Agent::AsyncTaskStarted(void* task) {
  client_->AsyncTaskStarted(task);
}

void Agent::AsyncTaskFinished(void* task) {
  client_->AsyncTaskFinished(task);
}

void Agent::AllAsyncTasksCanceled() {
  client_->AllAsyncTasksCanceled();
}

void Agent::ContextCreated(Local<Context> context, const ContextInfo& info) {
  if (client_ == nullptr)  // This happens for a main context
    return;
  client_->contextCreated(context, info);
}

bool Agent::IsActive() {
  if (client_ == nullptr)
    return false;
  return io_ != nullptr || client_->IsActive();
}

void Agent::WaitForConnect() {
  if (!client_) {
    ThrowUninitializedInspectorError(parent_env_);
    return;
  }

  CHECK_NOT_NULL(client_);
  client_->waitForFrontend();
}

std::string Agent::GetWsUrl() const {
  if (io_ == nullptr)
    return "";
  return io_->GetWsUrl();
}

SameThreadInspectorSession::~SameThreadInspectorSession() {
  auto client = client_.lock();
  if (client)
    client->disconnectFrontend(session_id_);
}

void SameThreadInspectorSession::Dispatch(
    const v8_inspector::StringView& message) {
  auto client = client_.lock();
  if (client)
    client->dispatchMessageFromFrontend(session_id_, message);
}

}  // namespace v8impl
