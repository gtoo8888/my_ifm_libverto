#ifndef _IFM_VERTO_MODULE_TYPES
#define _IFM_VERTO_MODULE_TYPES

#include "ifm_verto.h"


#include <sys/types.h>
typedef pid_t verto_proc;
typedef int verto_proc_status;


typedef void verto_mod_ctx;
typedef void verto_mod_ev;

#define VERTO_MODULE_VERSION 3
#define VERTO_MODULE_TABLE(name) verto_module_table_ ## name
#define VERTO_MODULE(name, symb, types) \
    static verto_ctx_funcs name ## _funcs = { \
        name ## _ctx_new, \
        name ## _ctx_default, \
        name ## _ctx_free, \
        name ## _ctx_run, \
        name ## _ctx_run_once, \
        name ## _ctx_break, \
        name ## _ctx_reinitialize, \
        name ## _ctx_set_flags, \
        name ## _ctx_add, \
        name ## _ctx_del \
    }; \
    verto_module VERTO_MODULE_TABLE(name) = { \
        VERTO_MODULE_VERSION, \
        # name, \
        # symb, \
        types, \
        &name ## _funcs, \
    }; \
    verto_ctx * \
    verto_new_ ## name() \
    { \
        return verto_convert(name, 0, NULL); \
    } \
    verto_ctx * \
    verto_default_ ## name() \
    { \
        return verto_convert(name, 1, NULL); \
    }

typedef struct {
    /* Required */ verto_mod_ctx *(*ctx_new)();
    /* Optional */ verto_mod_ctx *(*ctx_default)();
    /* Required */ void (*ctx_free)(verto_mod_ctx *ctx);
    /* Optional */ void (*ctx_run)(verto_mod_ctx *ctx);
    /* Required */ void (*ctx_run_once)(verto_mod_ctx *ctx);
    /* Optional */ void (*ctx_break)(verto_mod_ctx *ctx);
    /* Optional */ void (*ctx_reinitialize)(verto_mod_ctx *ctx);
    /* Optional */ void (*ctx_set_flags)(verto_mod_ctx *ctx,
                                         const verto_ev *ev,
                                         verto_mod_ev *modev);
    /* Required */ verto_mod_ev *(*ctx_add)(verto_mod_ctx *ctx,
                                            const verto_ev *ev,
                                            verto_ev_flag *flags);
    /* Required */ void (*ctx_del)(verto_mod_ctx *ctx,
                                   const verto_ev *ev,
                                   verto_mod_ev *modev);
} verto_ctx_funcs;

typedef struct {
    unsigned int vers;
    const char *name;
    const char *symb;
    verto_ev_type types;
    verto_ctx_funcs *funcs;
} verto_module;






#endif