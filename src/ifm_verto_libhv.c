
#include "ifm_verto_libhv.h"
#define VERTO_MODULE_TYPES
typedef struct hloop_t verto_mod_ctx;
typedef struct hevent_t verto_mod_ev;
#include "ifm_verto_module.h"

#include <hv/hloop.h>

// static verto_mod_ctx *
// libhv_ctx_new(void)
// {
//     hloop_t* loop;
    
//     loop = hloop_new(0);
//     // hevent_set_priority
//     // event_base_priority_init(eb, 3);
//     return loop;
// }

static verto_mod_ctx *
libhv_ctx_default(void)
{
    hloop_t* loop;
    
    loop = hloop_new(0);
    hevent_set_priority(loop, 0);
    return loop;
}


static void
libhv_ctx_free(verto_mod_ctx *ctx)
{
    hloop_free(&ctx);
}

static void
libhv_ctx_run(verto_mod_ctx *ctx)
{
    hloop_run(ctx);
}


static void
libhv_ctx_break(verto_mod_ctx *ctx)
{
    hloop_stop(ctx);
}

static void
libhv_ctx_del(verto_mod_ctx *ctx, const verto_ev *ev, verto_mod_ev *evpriv)
{
    // hio_del(ctx);
}


#define libhv_ctx_new NULL
#define libhv_ctx_run_once NULL
#define libhv_ctx_reinitialize NULL
#define libhv_ctx_set_flags NULL
#define libhv_ctx_add NULL
VERTO_MODULE(libhv, event_base_init,
             VERTO_EV_TYPE_IO |
             VERTO_EV_TYPE_TIMEOUT |
             VERTO_EV_TYPE_SIGNAL);
