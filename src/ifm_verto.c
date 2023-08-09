#include "ifm_verto.h"
#include "ifm_verto_module.h"



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

static module_record *loaded_modules;



///////////////////////////////////////////////////////////////////////////////////////////////////














verto_ctx *
verto_default(const char *impl, verto_ev_type reqtypes)
{
    // module_record *mr = NULL;

    // if (!load_module(impl, reqtypes, &mr))
    //     return NULL;

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
