
#ifndef __MEOW_HOOKS_H__
#define __MEOW_HOOKS_H__

#include <ctype.h>
#include <string.h>
#include <errno.h>

#include <pthread.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl31.h>
#include <GLES3/gl32.h>

#include "misc.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * GL / EGL prototype & hooks
 */
#define API_ENTRY(_r, _api, ...) typedef _r (*(MEOW_fp_##_api))(__VA_ARGS__);
    #include "./generated/egl_entries.in"
    #include "./generated/gl_entries.in"
#undef API_ENTRY

#define API_ENTRY(_r, _api, ...) MEOW_fp_##_api _api;
struct MEOW_egl_t {
    #include "./generated/egl_entries.in"
};
struct MEOW_gl_t {
    #include "./generated/gl_entries.in"
};
#undef API_ENTRY

/*
 * hook names
 */
char const * const *MEOW_get_egl_names();
char const * const *MEOW_get_gl_names();

typedef __eglMustCastToProperFunctionPointerType EGLFuncPointer;
typedef EGLFuncPointer (* getProcAddressType)(const char*);

struct MEOW_hook_t {
    MEOW_egl_t    egl;
    MEOW_gl_t     gl[2];
};

void MEOW_noop();

typedef int MEOW_API_register_tls();
typedef void MEOW_API_unregister_tls(int id);
typedef bool MEOW_API_thread_update_callchain(const struct MEOW_plugin_t* plugin, int mask);
typedef const char *MEOW_API_get_process_name();
typedef const char *MEOW_API_get_setting(const char* category, const char* key);
typedef MEOW_hook_t *MEOW_API_get_ddk_hook();
typedef int MEOW_API_log(int prio, const char *tag,  const char *fmt, ...);

#define MEOW_is_noop(f) ((void *)MEOW_noop == (void *)(f))
#define MEOW_NO_DATA (-1)

struct MEOW_plugin_t {
    /*
     * Called in zygote process typically, so query
     * Process_Name and PID are useless in this function.
     *
     * You can do properly initialization to take the advantage of Android's preload feature.
     *
     * @return 0, success.
     * @return != 0, fail or error to stop loading this plugin
     *   and you will not get any process/thread_*() callback after this call.
     */
    int (*init)();

    /*
     * Called when dlclose() happened or init() return error immediately.
     * Pair off with process_init().
     *
     * Always be called even the init/process_init() return fail.
     */
    void (*deinit)();

    /*
     * When pid is changed, aka fork(), the function is triggered.
     *
     * There is no process_deinit() since it is equal to the meaning of deinit().
     *
     * @return 0, success.
     * @return != 1, disable for this process,
     *   and you will not get any thread_*() callback after this call.
     */
    int (*process_init)();

    /*
     * Called when a EGL API is called on a thread first time.
     *
     * @return 0, success.
     * @return != 1, disable for this thread.
     */
    int (*thread_init)();

    /*
     * Called when thread is destroyed.
     * Pair off with thread_init().
     *
     * Always be called even the thread_init() return fail.
     */
    void (*thread_deinit)();

    /*
     * api_ver: INDEX_GLESv1 or INDEX_GLESv2 for gl*. (ignore for egl*.)
     *
     * return nullptr, if no need to wrapper the given function.
     */
    EGLFuncPointer (*get_address)(const char * func_name, int api_ver);

    int hook_mask = MEOW_NO_DATA;

    MEOW_API_register_tls* meow_register_tls;
    MEOW_API_unregister_tls* meow_unregister_tls;
    MEOW_API_thread_update_callchain* meow_thread_update_callchain;
    MEOW_API_get_process_name* meow_get_process_name;
    MEOW_API_get_ddk_hook* meow_get_ddk_hook;
    MEOW_API_get_setting* meow_get_setting;
    MEOW_API_log* meow_log;

    int *reserved[10];
};

#define MEOW_PLUGIN_SYM_STR "MEOW_PLUGIN_T_SYM"
#define MEOW_PLUGIN_SYM     MEOW_PLUGIN_T_SYM

#ifdef __cplusplus
}
#endif

#endif /* __MEOW_HOOKS_H__ */
