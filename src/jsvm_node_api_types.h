#ifndef SRC_NODE_API_TYPES_H_
#define SRC_NODE_API_TYPES_H_

#include "jsvm_types.h"

typedef struct jsvm_callback_scope__* jsvm_callback_scope;
typedef struct jsvm_async_context__* jsvm_async_context;
typedef struct jsvm_async_work__* jsvm_async_work;

#if NAPI_VERSION >= 3
typedef void(JSVM_CDECL* jsvm_cleanup_hook)(void* arg);
#endif  // NAPI_VERSION >= 3

#if NAPI_VERSION >= 4
typedef struct jsvm_threadsafe_function__* jsvm_threadsafe_function;
#endif  // NAPI_VERSION >= 4

#if NAPI_VERSION >= 4
typedef enum {
  jsvm_tsfn_release,
  jsvm_tsfn_abort
} jsvm_threadsafe_function_release_mode;

typedef enum {
  jsvm_tsfn_nonblocking,
  napi_tsfn_blocking
} jsvm_threadsafe_function_call_mode;
#endif  // NAPI_VERSION >= 4

typedef void(JSVM_CDECL* jsvm_async_execute_callback)(JSVM_Env env, void* data);
typedef void(JSVM_CDECL* jsvm_async_complete_callback)(JSVM_Env env,
                                                       JSVM_Status status,
                                                       void* data);
#if NAPI_VERSION >= 4
typedef void(JSVM_CDECL* jsvm_threadsafe_function_call_js)(
    JSVM_Env env, JSVM_Value js_callback, void* context, void* data);
#endif  // NAPI_VERSION >= 4

typedef struct {
  uint32_t major;
  uint32_t minor;
  uint32_t patch;
  const char* release;
} jsvm_node_version;

#if NAPI_VERSION >= 8
typedef struct jsvm_async_cleanup_hook_handle__* jsvm_async_cleanup_hook_handle;
typedef void(JSVM_CDECL* jsvm_async_cleanup_hook)(
    jsvm_async_cleanup_hook_handle handle, void* data);
#endif  // NAPI_VERSION >= 8

#endif  // SRC_NODE_API_TYPES_H_
