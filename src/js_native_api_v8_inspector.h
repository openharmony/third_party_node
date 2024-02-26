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

#pragma once

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#if !HAVE_INSPECTOR
#error("This header can only be used when inspector is enabled")
#endif

#include "node_options.h"
#include "v8.h"

#include <cstddef>
#include <memory>

namespace v8_inspector {
class StringView;
}  // namespace v8_inspector

namespace node {
namespace inspector {

class ParentInspectorHandle;
class WorkerManager;
class InspectorSessionDelegate;
class InspectorSession;

}  // namespace inspector
}  // namespace node

namespace v8impl {

using node::ExclusiveAccess;
using node::HostPort;
using node::inspector::InspectorSession;
using node::inspector::InspectorSessionDelegate;
using node::inspector::ParentInspectorHandle;
using node::inspector::WorkerManager;

class IsolateData;
using Environment = JSVM_Env__;

class InspectorClient;
class InspectorIo;

struct ContextInfo {
  explicit ContextInfo(const std::string& name) : name(name) {}
  const std::string name;
  std::string origin;
  bool is_default = false;
};

class Agent {
 public:
  explicit Agent(Environment* env);
  ~Agent();

  // Create client_, may create io_ if option enabled
  bool Start(const std::string& path,
             std::shared_ptr<ExclusiveAccess<HostPort>> host_port,
             bool is_main,
             bool wait_for_connect);
  // Stop and destroy io_
  void Stop();

  bool IsListening() { return io_ != nullptr; }
  // Returns true if the Node inspector is actually in use. It will be true
  // if either the user explicitly opted into inspector (e.g. with the
  // --inspect command line flag) or if inspector JS API had been used.
  bool IsActive();

  // Blocks till frontend connects and sends "runIfWaitingForDebugger"
  void WaitForConnect();
  // Blocks till all the sessions with "WaitForDisconnectOnShutdown" disconnect
  void WaitForDisconnect();
  void ReportUncaughtException(v8::Local<v8::Value> error,
                               v8::Local<v8::Message> message);

  // Async stack traces instrumentation.
  void AsyncTaskScheduled(const v8_inspector::StringView& taskName, void* task,
                          bool recurring);
  void AsyncTaskCanceled(void* task);
  void AsyncTaskStarted(void* task);
  void AsyncTaskFinished(void* task);
  void AllAsyncTasksCanceled();

  // Called to create inspector sessions that can be used from the same thread.
  // The inspector responds by using the delegate to send messages back.
  std::unique_ptr<InspectorSession> Connect(
      std::unique_ptr<InspectorSessionDelegate> delegate,
      bool prevent_shutdown);

  void PauseOnNextJavascriptStatement(const std::string& reason);

  std::string GetWsUrl() const;

  // Can only be called from the main thread.
  bool StartIoThread();

  std::shared_ptr<ExclusiveAccess<HostPort>> host_port() { return host_port_; }
  void ContextCreated(v8::Local<v8::Context> context, const ContextInfo& info);

  inline Environment* env() const { return parent_env_; }

 private:
  Environment* parent_env_;
  // Encapsulates majority of the Inspector functionality
  std::shared_ptr<InspectorClient> client_;
  // Interface for transports, e.g. WebSocket server
  std::unique_ptr<InspectorIo> io_;
  std::string path_;

  std::shared_ptr<ExclusiveAccess<HostPort>> host_port_;
};

}  // namespace v8impl

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS
