#ifndef SRC_NODE_API_H_
#define SRC_NODE_API_H_

#ifdef BUILDING_NODE_EXTENSION
#ifdef _WIN32
// Building native addon against node
#define JSVM_EXTERN __declspec(dllimport)
#elif defined(__wasm32__)
#define JSVM_EXTERN __attribute__((__import_module__("napi")))
#endif
#endif
#include "jsvm.h"
#include "jsvm_node_api_types.h"

struct uv_loop_s;  // Forward declaration.

#ifdef _WIN32
#define JSVM_MODULE_EXPORT __declspec(dllexport)
#else
#define JSVM_MODULE_EXPORT __attribute__((visibility("default")))
#endif

#if defined(__GNUC__)
#define JSVM_NO_RETURN __attribute__((noreturn))
#elif defined(_WIN32)
#define JSVM_NO_RETURN __declspec(noreturn)
#else
#define JSVM_NO_RETURN
#endif

typedef JSVM_Value(JSVM_CDECL* jsvm_addon_register_func)(JSVM_Env env,
                                                         JSVM_Value exports);
typedef int32_t(JSVM_CDECL* node_api_addon_get_api_version_func)();

// Used by deprecated registration method jsvm_module_register.
typedef struct jsvm_module {
  int nm_version;
  unsigned int nm_flags;
  const char* nm_filename;
  jsvm_addon_register_func nm_register_func;
  const char* nm_modname;
  void* nm_priv;
  void* reserved[4];
} jsvm_module;

#define JSVM_MODULE_VERSION 1

#define JSVM_MODULE_INITIALIZER_X(base, version)                               \
  JSVM_MODULE_INITIALIZER_X_HELPER(base, version)
#define JSVM_MODULE_INITIALIZER_X_HELPER(base, version) base##version

#ifdef __wasm32__
#define JSVM_MODULE_INITIALIZER_BASE jsvm_register_wasm_v
#else
#define JSVM_MODULE_INITIALIZER_BASE jsvm_register_module_v
#endif

#define NODE_API_MODULE_GET_API_VERSION_BASE node_api_module_get_api_version_v

#define JSVM_MODULE_INITIALIZER                                                \
  JSVM_MODULE_INITIALIZER_X(JSVM_MODULE_INITIALIZER_BASE, JSVM_MODULE_VERSION)

#define NODE_API_MODULE_GET_API_VERSION                                        \
  JSVM_MODULE_INITIALIZER_X(NODE_API_MODULE_GET_API_VERSION_BASE,              \
                            JSVM_MODULE_VERSION)

#define JSVM_MODULE_INIT()                                                     \
  EXTERN_C_START                                                               \
  JSVM_MODULE_EXPORT int32_t NODE_API_MODULE_GET_API_VERSION() {               \
    return NAPI_VERSION;                                                       \
  }                                                                            \
  JSVM_MODULE_EXPORT JSVM_Value JSVM_MODULE_INITIALIZER(JSVM_Env env,          \
                                                        JSVM_Value exports);   \
  EXTERN_C_END                                                                 \
  JSVM_Value JSVM_MODULE_INITIALIZER(JSVM_Env env, JSVM_Value exports)

#define JSVM_MODULE(modname, regfunc)                                          \
  JSVM_MODULE_INIT() { return regfunc(env, exports); }

// Deprecated. Use JSVM_MODULE.
#define JSVM_MODULE_X(modname, regfunc, priv, flags)                           \
  JSVM_MODULE(modname, regfunc)

EXTERN_C_START

// Deprecated. Replaced by symbol-based registration defined by JSVM_MODULE
// and JSVM_MODULE_INIT macros.
#if defined(__cplusplus) && __cplusplus >= 201402L
[[deprecated]]
#endif
JSVM_EXTERN void JSVM_CDECL
jsvm_module_register(jsvm_module* mod);

JSVM_EXTERN JSVM_NO_RETURN void JSVM_CDECL
jsvm_fatal_error(const char* location,
                 size_t location_len,
                 const char* message,
                 size_t message_len);

// Methods for custom handling of async operations
JSVM_EXTERN JSVM_Status JSVM_CDECL
jsvm_async_init(JSVM_Env env,
                JSVM_Value async_resource,
                JSVM_Value async_resource_name,
                jsvm_async_context* result);

JSVM_EXTERN JSVM_Status JSVM_CDECL
jsvm_async_destroy(JSVM_Env env, jsvm_async_context async_context);

JSVM_EXTERN JSVM_Status JSVM_CDECL
jsvm_make_callback(JSVM_Env env,
                   jsvm_async_context async_context,
                   JSVM_Value recv,
                   JSVM_Value func,
                   size_t argc,
                   const JSVM_Value* argv,
                   JSVM_Value* result);

// Methods to provide node::Buffer functionality with napi types
JSVM_EXTERN JSVM_Status JSVM_CDECL jsvm_create_buffer(JSVM_Env env,
                                                      size_t length,
                                                      void** data,
                                                      JSVM_Value* result);
#ifndef NODE_API_NO_EXTERNAL_BUFFERS_ALLOWED
JSVM_EXTERN JSVM_Status JSVM_CDECL
OH_JSVM_CreateExternal_buffer(JSVM_Env env,
                            size_t length,
                            void* data,
                            JSVM_Finalize finalizeCb,
                            void* finalizeHint,
                            JSVM_Value* result);
#endif  // NODE_API_NO_EXTERNAL_BUFFERS_ALLOWED
JSVM_EXTERN JSVM_Status JSVM_CDECL jsvm_create_buffer_copy(JSVM_Env env,
                                                           size_t length,
                                                           const void* data,
                                                           void** result_data,
                                                           JSVM_Value* result);
JSVM_EXTERN JSVM_Status JSVM_CDECL jsvm_is_buffer(JSVM_Env env,
                                                  JSVM_Value value,
                                                  bool* result);
JSVM_EXTERN JSVM_Status JSVM_CDECL jsvm_get_buffer_info(JSVM_Env env,
                                                        JSVM_Value value,
                                                        void** data,
                                                        size_t* length);

#ifndef __wasm32__
// Methods to manage simple async operations
JSVM_EXTERN JSVM_Status JSVM_CDECL
jsvm_create_async_work(JSVM_Env env,
                       JSVM_Value async_resource,
                       JSVM_Value async_resource_name,
                       jsvm_async_execute_callback execute,
                       jsvm_async_complete_callback complete,
                       void* data,
                       jsvm_async_work* result);
JSVM_EXTERN JSVM_Status JSVM_CDECL jsvm_delete_async_work(JSVM_Env env,
                                                          jsvm_async_work work);
JSVM_EXTERN JSVM_Status JSVM_CDECL jsvm_queue_async_work(JSVM_Env env,
                                                         jsvm_async_work work);
JSVM_EXTERN JSVM_Status JSVM_CDECL jsvm_cancel_async_work(JSVM_Env env,
                                                          jsvm_async_work work);
#endif  // __wasm32__

// version management
JSVM_EXTERN JSVM_Status JSVM_CDECL
jsvm_get_node_version(JSVM_Env env, const jsvm_node_version** version);

#if NAPI_VERSION >= 2

// Return the current libuv event loop for a given environment
JSVM_EXTERN JSVM_Status JSVM_CDECL
jsvm_get_uv_event_loop(JSVM_Env env, struct uv_loop_s** loop);

#endif  // NAPI_VERSION >= 2

#if NAPI_VERSION >= 3

JSVM_EXTERN JSVM_Status JSVM_CDECL jsvm_fatal_exception(JSVM_Env env,
                                                        JSVM_Value err);

JSVM_EXTERN JSVM_Status JSVM_CDECL
jsvm_add_env_cleanup_hook(JSVM_Env env, jsvm_cleanup_hook fun, void* arg);

JSVM_EXTERN JSVM_Status JSVM_CDECL
jsvm_remove_env_cleanup_hook(JSVM_Env env, jsvm_cleanup_hook fun, void* arg);

JSVM_EXTERN JSVM_Status JSVM_CDECL
jsvm_open_callback_scope(JSVM_Env env,
                         JSVM_Value resource_object,
                         jsvm_async_context context,
                         jsvm_callback_scope* result);

JSVM_EXTERN JSVM_Status JSVM_CDECL
jsvm_close_callback_scope(JSVM_Env env, jsvm_callback_scope scope);

#endif  // NAPI_VERSION >= 3

#if NAPI_VERSION >= 4

#ifndef __wasm32__
// Calling into JS from other threads
JSVM_EXTERN JSVM_Status JSVM_CDECL
jsvm_create_threadsafe_function(JSVM_Env env,
                                JSVM_Value func,
                                JSVM_Value async_resource,
                                JSVM_Value async_resource_name,
                                size_t max_queue_size,
                                size_t initial_thread_count,
                                void* thread_finalize_data,
                                JSVM_Finalize thread_finalize_cb,
                                void* context,
                                jsvm_threadsafe_function_call_js call_js_cb,
                                jsvm_threadsafe_function* result);

JSVM_EXTERN JSVM_Status JSVM_CDECL jsvm_get_threadsafe_function_context(
    jsvm_threadsafe_function func, void** result);

JSVM_EXTERN JSVM_Status JSVM_CDECL
jsvm_call_threadsafe_function(jsvm_threadsafe_function func,
                              void* data,
                              jsvm_threadsafe_function_call_mode is_blocking);

JSVM_EXTERN JSVM_Status JSVM_CDECL
jsvm_acquire_threadsafe_function(jsvm_threadsafe_function func);

JSVM_EXTERN JSVM_Status JSVM_CDECL jsvm_release_threadsafe_function(
    jsvm_threadsafe_function func, jsvm_threadsafe_function_release_mode mode);

JSVM_EXTERN JSVM_Status JSVM_CDECL
jsvm_unref_threadsafe_function(JSVM_Env env, jsvm_threadsafe_function func);

JSVM_EXTERN JSVM_Status JSVM_CDECL
jsvm_ref_threadsafe_function(JSVM_Env env, jsvm_threadsafe_function func);
#endif  // __wasm32__

#endif  // NAPI_VERSION >= 4

#if NAPI_VERSION >= 8

JSVM_EXTERN JSVM_Status JSVM_CDECL
jsvm_add_async_cleanup_hook(JSVM_Env env,
                            jsvm_async_cleanup_hook hook,
                            void* arg,
                            jsvm_async_cleanup_hook_handle* remove_handle);

JSVM_EXTERN JSVM_Status JSVM_CDECL
jsvm_remove_async_cleanup_hook(jsvm_async_cleanup_hook_handle remove_handle);

#endif  // NAPI_VERSION >= 8

#if NAPI_VERSION >= 9

JSVM_EXTERN JSVM_Status JSVM_CDECL
node_api_get_module_file_name(JSVM_Env env, const char** result);

#endif  // NAPI_VERSION >= 9

EXTERN_C_END

#endif  // SRC_NODE_API_H_
