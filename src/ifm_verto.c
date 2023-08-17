#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "ifm_verto.h"
#include "ifm_verto_module.h"
#include "module.h"

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
    verto_callback *callback;
    verto_callback *onfree;
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


static void *(*resize_cb)(void *mem, size_t size);
#define vfree(mem) vresize(mem, 0)
static void *
vresize(void *mem, size_t size)
{
    if (!resize_cb)
        resize_cb = &realloc;
    if (size == 0 && resize_cb == &realloc) {
        /* Avoid memleak as realloc(X, 0) can return a free-able pointer. */
        free(mem);
        return NULL;
    }
    return (*resize_cb)(mem, size);
}

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
    char *prefix = NULL;
    char *suffix = NULL;
    char *tmp = NULL;
#endif

    static module_record *t1 = &builtin_record;
    static module_record *loaded_modules2 = &builtin_record;
    /* Check the cache */
    mutex_lock(&loaded_modules_mutex);
    if (impl) {
        for (*record = loaded_modules2 ; *record ; *record = (*record)->next) {
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


static void
remove_ev(verto_ev **origin, verto_ev *item)
{
    if (!origin || !*origin || !item)
        return;

    if (*origin == item)
        *origin = (*origin)->next;
    else
        remove_ev(&((*origin)->next), item);
}



verto_ctx *
verto_convert_module(const verto_module *module, int deflt, verto_mod_ctx *mctx)
{
    verto_ctx *ctx = NULL;
    module_record *mr;

    if (!module)
        return NULL;

    if (deflt) {
        mutex_lock(&loaded_modules_mutex);
        for (mr = loaded_modules ; mr ; mr = mr->next) {
            verto_ctx *tmp;
            if (mr->module == module && mr->defctx) {
                if (mctx)
                    module->funcs->ctx_free(mctx);
                tmp = mr->defctx;
                tmp->ref++;
                mutex_unlock(&loaded_modules_mutex);
                return tmp;
            }
        }
        mutex_unlock(&loaded_modules_mutex);
    }

    if (!mctx) {
        mctx = deflt
                    ? (module->funcs->ctx_default
                        ? module->funcs->ctx_default()
                        : module->funcs->ctx_new())
                    : module->funcs->ctx_new();
        if (!mctx)
            goto error;
    }

    ctx = vresize(NULL, sizeof(verto_ctx));
    if (!ctx)
        goto error;
    memset(ctx, 0, sizeof(verto_ctx));

    ctx->ref = 1;
    ctx->ctx = mctx;
    ctx->module = module;
    ctx->deflt = deflt;

    if (deflt) {
        module_record **tmp;

        mutex_lock(&loaded_modules_mutex);
        tmp = &loaded_modules;
        for (mr = loaded_modules ; mr ; mr = mr->next) {
            if (mr->module == module) {
                assert(mr->defctx == NULL);
                mr->defctx = ctx;
                mutex_unlock(&loaded_modules_mutex);
                return ctx;
            }

            if (!mr->next) {
                tmp = &mr->next;
                break;
            }
        }
        mutex_unlock(&loaded_modules_mutex);

        *tmp = vresize(NULL, sizeof(module_record));
        if (!*tmp) {
            vfree(ctx);
            goto error;
        }

        memset(*tmp, 0, sizeof(module_record));
        (*tmp)->defctx = ctx;
        (*tmp)->module = module;
    }

    return ctx;

error:
    if (mctx)
        module->funcs->ctx_free(mctx);
    return NULL;
}






verto_ctx *
verto_default(const char *impl, verto_ev_type reqtypes)
{
    module_record *mr = NULL;

    if (!load_module(impl, reqtypes, &mr))
        return NULL;

    return verto_convert_module(mr->module, 1, NULL);
}


void
verto_run(verto_ctx *ctx)
{
    if (!ctx)
        return;

    if (ctx->module->funcs->ctx_break && ctx->module->funcs->ctx_run)
        ctx->module->funcs->ctx_run(ctx->ctx);
    else {
        while (!ctx->exit)
            ctx->module->funcs->ctx_run_once(ctx->ctx);
        ctx->exit = 0;
    }
}


void
verto_break(verto_ctx *ctx)
{
    if (!ctx)
        return;

    if (ctx->module->funcs->ctx_break && ctx->module->funcs->ctx_run)
        ctx->module->funcs->ctx_break(ctx->ctx);
    else
        ctx->exit = 1;
}

void
verto_del(verto_ev *ev)
{
    if (!ev)
        return;

    /* If the event is freed in the callback, we just set a flag so that
     * verto_fire() can actually do the delete when the callback completes.
     *
     * If we don't do this, than verto_fire() will access freed memory. */
    if (ev->depth > 0) {
        ev->deleted = 1;
        return;
    }

    if (ev->onfree)
        ev->onfree(ev->ctx, ev);
    ev->ctx->module->funcs->ctx_del(ev->ctx->ctx, ev, ev->ev);
    remove_ev(&(ev->ctx->events), ev);

    if ((ev->type == VERTO_EV_TYPE_IO) &&
        (ev->flags & VERTO_EV_FLAG_IO_CLOSE_FD) &&
        !(ev->actual & VERTO_EV_FLAG_IO_CLOSE_FD))
        close(ev->option.io.fd);

    vfree(ev);
}

void
verto_free(verto_ctx *ctx)
{
    verto_ev *cur, *next;

    if (!ctx)
        return;

    ctx->ref = ctx->ref > 0 ? ctx->ref - 1 : 0;
    if (ctx->ref > 0)
        return;

    /* Cancel all pending events */
    next = NULL;
    for (cur = ctx->events; cur != NULL; cur = next) {
        next = cur->next;
        verto_del(cur);
    }
    ctx->events = NULL;

    /* Free the private */
    if (!ctx->deflt || !ctx->module->funcs->ctx_default)
        ctx->module->funcs->ctx_free(ctx->ctx);

    vfree(ctx);
}

void
verto_cleanup(void)
{
    module_record *record;

    mutex_lock(&loaded_modules_mutex);

    for (record = loaded_modules; record; record = record->next) {
        module_close(record->dll);
        free(record->filename);
    }

    vfree(loaded_modules);
    loaded_modules = NULL;

    mutex_unlock(&loaded_modules_mutex);
    mutex_destroy(&loaded_modules_mutex);
}


static verto_ev *
make_ev(verto_ctx *ctx, verto_callback *callback,
        verto_ev_type type, verto_ev_flag flags)
{
    verto_ev *ev = NULL;

    if (!ctx || !callback)
        return NULL;

    ev = vresize(NULL, sizeof(verto_ev));
    if (ev) {
        memset(ev, 0, sizeof(verto_ev));
        ev->ctx        = ctx;
        ev->type       = type;
        ev->callback   = callback;
        ev->flags      = flags;
    }

    return ev;
}

static void
push_ev(verto_ctx *ctx, verto_ev *ev)
{
    verto_ev *tmp;

    if (!ctx || !ev)
        return;

    tmp = ctx->events;
    ctx->events = ev;
    ctx->events->next = tmp;
}

/* Remove flags we can emulate */
// 从给定的标志中移除可以模拟的标志
// 移除了VERTO_EV_FLAG_PERSIST和VERTO_EV_FLAG_IO_CLOSE_FD标志位
#define make_actual(flags) ((flags) & ~(VERTO_EV_FLAG_PERSIST|VERTO_EV_FLAG_IO_CLOSE_FD))

#define doadd(ev, set, type) \
    ev = make_ev(ctx, callback, type, flags); \
    if (ev) { \
        set; \
        ev->actual = make_actual(ev->flags); \
        ev->ev = ctx->module->funcs->ctx_add(ctx->ctx, ev, &ev->actual); \
        if (!ev->ev) { \
            vfree(ev); \
            return NULL; \
        } \
        push_ev(ctx, ev); \
    }

verto_ev *
verto_add_signal(verto_ctx *ctx, verto_ev_flag flags,
                 verto_callback *callback, int signal)
{
    verto_ev *ev;

    if (signal < 0)
        return NULL;
        
    // if (callback == VERTO_SIG_IGN) {
    //     callback = signal_ignore;
    //     if (!(flags & VERTO_EV_FLAG_PERSIST))
    //         return NULL;
    // }
    doadd(ev, ev->option.signal = signal, VERTO_EV_TYPE_SIGNAL);
    return ev;
}

verto_ev *
verto_add_io(verto_ctx *ctx, verto_ev_flag flags,
             verto_callback *callback, int fd)
{
    verto_ev *ev;

    if (fd < 0 || !(flags & (VERTO_EV_FLAG_IO_READ | VERTO_EV_FLAG_IO_WRITE)))
        return NULL;

    doadd(ev, ev->option.io.fd = fd, VERTO_EV_TYPE_IO);
    return ev;
}

verto_ev *
verto_add_timeout(verto_ctx *ctx, verto_ev_flag flags,
                  verto_callback *callback, time_t interval)
{
    verto_ev *ev;
    doadd(ev, ev->option.interval = interval, VERTO_EV_TYPE_TIMEOUT);
    return ev;
}


verto_ev_type
verto_get_type(const verto_ev *ev)
{
    return ev->type;
}

time_t
verto_get_interval(const verto_ev *ev)
{
    if (ev && (ev->type == VERTO_EV_TYPE_TIMEOUT))
        return ev->option.interval;
    return 0;
}

void *
verto_get_private(const verto_ev *ev)
{
    return ev->priv;
}


int
verto_get_fd(const verto_ev *ev)
{
    if (ev && (ev->type == VERTO_EV_TYPE_IO))
        return ev->option.io.fd;
    return -1;
}
