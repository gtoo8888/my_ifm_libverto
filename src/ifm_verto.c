#include "ifm_verto.h"
#include "ifm_verto_module.h"

#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>


struct verto_ctx {
    size_t ref;
    verto_mod_ctx *ctx;
    const verto_module *module;
    verto_ev *events;
    int deflt;
    int exit;
};

typedef struct {
    verto_proc proc;
    verto_proc_status status;
} verto_child;

typedef struct {
    int fd;
    verto_ev_flag state;
} verto_io;

struct verto_ev {
    verto_ev *next;
    verto_ctx *ctx;
    verto_ev_type type;
    // verto_callback *callback;
    // verto_callback *onfree;
    void *priv;
    verto_mod_ev *ev;
    verto_ev_flag flags;
    verto_ev_flag actual;
    size_t depth;
    int deleted;
    union {
        verto_io io;
        int signal;
        time_t interval;
        verto_child child;
    } option;
};


typedef struct module_record module_record;
struct module_record {
    module_record *next;
    const verto_module *module;
    void *dll;
    char *filename;
    verto_ctx *defctx;
};

#ifdef BUILTIN_MODULE
#define _MODTABLE(n) verto_module_table_ ## n
#define MODTABLE(n) _MODTABLE(n)
/*
 * This symbol can be used when embedding verto.c in a library along with a
 * built-in private module, to preload the module instead of dynamically
 * linking it in later.  Define to <modulename>.
 */
extern verto_module MODTABLE(BUILTIN_MODULE);
static module_record builtin_record = {
    NULL, &MODTABLE(BUILTIN_MODULE), NULL, "", NULL
};
static module_record *loaded_modules = &builtin_record;
#else
static module_record *loaded_modules;
#endif



///////////////////////////////////////////////////////////////////////////////////////////////////


static pthread_mutex_t loaded_modules_mutex = PTHREAD_MUTEX_INITIALIZER;

#define mutex_lock(x) { \
        int c = pthread_mutex_lock(x); \
        if (c != 0) { \
            fprintf(stderr, "pthread_mutex_lock returned %d (%s) in %s", \
                    c, strerror(c), __FUNCTION__); \
        } \
        assert(c == 0); \
    }
#define mutex_unlock(x) { \
        int c = pthread_mutex_unlock(x); \
        if (c != 0) { \
            fprintf(stderr, "pthread_mutex_unlock returned %d (%s) in %s", \
                    c, strerror(c), __FUNCTION__); \
        } \
        assert(c == 0); \
    }
#define mutex_destroy(x) { \
        int c = pthread_mutex_destroy(x); \
        if (c != 0) { \
            fprintf(stderr, "pthread_mutex_destroy returned %d (%s) in %s", \
                    c, strerror(c), __FUNCTION__); \
        } \
        assert(c == 0); \
    }


static int
load_module(const char *impl, verto_ev_type reqtypes, module_record **record)
{
    int success = 0;
#ifndef BUILTIN_MODULE
    // char *prefix = NULL;
    // char *suffix = NULL;
    // char *tmp = NULL;
#endif

    /* Check the cache */
    mutex_lock(&loaded_modules_mutex);
    if (impl) {
        for (*record = loaded_modules ; *record ; *record = (*record)->next) {
            if ((strchr(impl, '/') && !strcmp(impl, (*record)->filename))
                    || !strcmp(impl, (*record)->module->name)) {
                mutex_unlock(&loaded_modules_mutex);
                return 1;
            }
        }
    } else if (loaded_modules) {
        for (*record = loaded_modules ; *record ; *record = (*record)->next) {
            if (reqtypes == VERTO_EV_TYPE_NONE
                    || ((*record)->module->types & reqtypes) == reqtypes) {
                mutex_unlock(&loaded_modules_mutex);
                return 1;
            }
        }
    }
    mutex_unlock(&loaded_modules_mutex);

#ifndef BUILTIN_MODULE
    if (!module_get_filename_for_symbol(verto_convert_module, &prefix))
        return 0;

    /* Example output:
     *    prefix == /usr/lib/libverto-
     *    impl == glib
     *    suffix == .so.0
     * Put them all together: /usr/lib/libverto-glib.so.0 */
    tmp = strdup(prefix);
    if (!tmp) {
        free(prefix);
        return 0;
    }

    suffix = basename(tmp);
    suffix = strchr(suffix, '.');
    if (!suffix || strlen(suffix) < 1 || !(suffix = strdup(suffix))) {
        free(prefix);
        free(tmp);
        return 0;
    }
    strcpy(prefix + strlen(prefix) - strlen(suffix), "-");
    free(tmp);

    if (impl) {
        /* Try to do a load by the path */
        if (!success && strchr(impl, '/'))
            success = do_load_file(impl, 0, reqtypes, record);
        if (!success) {
            /* Try to do a load by the name */
            tmp = string_aconcat(prefix, impl, suffix);
            if (tmp) {
                success = do_load_file(tmp, 0, reqtypes, record);
                free(tmp);
            }
        }
    } else {
        /* NULL was passed, so we will use the dirname of
         * the prefix to try and find any possible plugins */
        tmp = strdup(prefix);
        if (tmp) {
            char *dname = strdup(dirname(tmp));
            free(tmp);

            tmp = strdup(basename(prefix));
            free(prefix);
            prefix = tmp;

            if (dname && prefix) {
                /* Attempt to find a module we are already linked to */
                success = do_load_dir(dname, prefix, suffix, 1, reqtypes,
                                      record);
                if (!success) {
#ifdef DEFAULT_MODULE
                    /* Attempt to find the default module */
                    success = load_module(DEFAULT_MODULE, reqtypes, record);
                    if (!success)
#endif /* DEFAULT_MODULE */
                        /* Attempt to load any plugin (we're desperate) */
                        success = do_load_dir(dname, prefix, suffix, 0,
                                              reqtypes, record);
                }
            }

            free(dname);
        }
    }

    free(suffix);
    free(prefix);
#endif /* BUILTIN_MODULE */
    return success;
}











verto_ctx *
verto_default(const char *impl, verto_ev_type reqtypes)
{
    module_record *mr = NULL;

    if (!load_module(impl, reqtypes, &mr))
        return NULL;

    // return verto_convert_module(mr->module, 1, NULL);
}


void
verto_run(verto_ctx *ctx)
{
    if (!ctx)
        return;

    // if (ctx->module->funcs->ctx_break && ctx->module->funcs->ctx_run)
    //     ctx->module->funcs->ctx_run(ctx->ctx);
    // else {
    //     while (!ctx->exit)
    //         ctx->module->funcs->ctx_run_once(ctx->ctx);
    //     ctx->exit = 0;
    // }
}


void
verto_break(verto_ctx *ctx)
{
    if (!ctx)
        return;

    // if (ctx->module->funcs->ctx_break && ctx->module->funcs->ctx_run)
    //     ctx->module->funcs->ctx_break(ctx->ctx);
    // else
    //     ctx->exit = 1;
}


void
verto_free(verto_ctx *ctx)
{
    // verto_ev *cur, *next;

    // if (!ctx)
    //     return;

    // ctx->ref = ctx->ref > 0 ? ctx->ref - 1 : 0;
    // if (ctx->ref > 0)
    //     return;

    // /* Cancel all pending events */
    // next = NULL;
    // for (cur = ctx->events; cur != NULL; cur = next) {
    //     next = cur->next;
    //     // verto_del(cur);
    // }
    // ctx->events = NULL;

    // /* Free the private */
    // if (!ctx->deflt || !ctx->module->funcs->ctx_default)
    //     ctx->module->funcs->ctx_free(ctx->ctx);

    // vfree(ctx);
}

