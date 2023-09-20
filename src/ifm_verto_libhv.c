#include "ifm_verto_libhv.h"
#include <hv/hloop.h>
#include <hv/hmain.h>
#include <signal.h>

#define VERTO_MODULE_TYPES
struct hevent2verto {
    hloop_t* loop;
    verto_ev* ev;
    hio_t *io;
    htimer_t *timer;
    int timeout;
};
typedef struct hloop_t verto_mod_ctx;
// typedef struct hevent_t verto_mod_ev;
typedef struct hevent2verto verto_mod_ev;
#include "ifm_verto_module.h"


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


static timer_count = 0; 

static void
libhv_callback_io(hio_t* io)
{
    timer_count++;
    printf("timer_count:%d\n",timer_count);
    struct hevent2verto *h2ve = (struct hevent2verto *)hevent_userdata(io);
    verto_ev_flag state = VERTO_EV_FLAG_NONE;

    if (verto_get_flags(h2ve->ev) & VERTO_EV_FLAG_IO_READ) {
        state |= VERTO_EV_FLAG_IO_READ;
    }
    if (verto_get_flags(h2ve->ev) & VERTO_EV_FLAG_IO_WRITE) {
        state |= VERTO_EV_FLAG_IO_WRITE;
    }
#ifdef EV_ERROR
    if (type & EV_ERROR)
        state |= VERTO_EV_FLAG_IO_ERROR;
#endif

    verto_set_fd_state(h2ve->ev, state);
    verto_fire(h2ve->ev);
}


static void
libhv_callback_timer(htimer_t* timer)
{
    timer_count++;
    printf("timer_count:%d\n",timer_count);
    struct hevent2verto *h2ve = (struct hevent2verto *)hevent_userdata(timer);
    verto_ev_flag state = VERTO_EV_FLAG_NONE;



//     if (type & HV_READ)
//         state |= VERTO_EV_FLAG_IO_READ;
//     if (type & HV_READ)
//         state |= VERTO_EV_FLAG_IO_WRITE;
// #ifdef EV_ERROR
//     if (type & EV_ERROR)
//         state |= VERTO_EV_FLAG_IO_ERROR;
// #endif

    verto_set_fd_state(h2ve->ev, state);
    verto_fire(h2ve->ev);
}



static void libhv_callback_signal(void* userdata) {
    // hlogi("reload confile [%s]", g_main_ctx.confile);
    // parse_confile(g_main_ctx.confile);
}

static verto_mod_ev *
libhv_ctx_add(verto_mod_ctx *ctx, const verto_ev *ev, verto_ev_flag *flags)
{
    struct hevent2verto *priv = NULL;
    priv = malloc(sizeof(struct hevent2verto*));
    priv->loop = ctx;
    priv->ev = ev;
    time_t time_interval;
    hio_t* io = NULL;
    int libeventflags = 0;

    *flags |= verto_get_flags(ev) & VERTO_EV_FLAG_PERSIST; // 控制actual,保证不重复调用
    // if (verto_get_flags(ev) & VERTO_EV_FLAG_PERSIST)
    //     libeventflags |= EV_PERSIST;

    int tmp;
    switch (verto_get_type(ev)) {
    case VERTO_EV_TYPE_IO:
        tmp = verto_get_fd(ev);
        priv->io = hio_get((hloop_t*)ctx, verto_get_fd(ev));
        if (verto_get_flags(ev) & VERTO_EV_FLAG_IO_READ) {
            hio_add(priv->io, (hio_cb)libhv_callback_io, HV_READ);
        }
        if (verto_get_flags(ev) & VERTO_EV_FLAG_IO_WRITE) {
            hio_add(priv->io, (hio_cb)libhv_callback_io, HV_WRITE);
        }
        if(priv->io != NULL){
            hevent_set_userdata(priv->io, priv);
        }
        break;
    case VERTO_EV_TYPE_TIMEOUT:
        time_interval = verto_get_interval(ev);
        priv->timer = (htimer_t*)htimer_add((hloop_t*)ctx, (htimer_cb)libhv_callback_timer, time_interval, INFINITE);
        if(priv->timer != NULL){
            hevent_set_userdata(priv->timer, priv);
        }
        break;
    case VERTO_EV_TYPE_SIGNAL:
        // hevent_t ev;
        // ev.cb = signal_event_cb;
        // // ...
        // hloop_post_event(loop, &ev);
        // signal(verto_get_signal(ev), signal_func);

        break;
    case VERTO_EV_TYPE_IDLE:
    case VERTO_EV_TYPE_CHILD:
    default:
        return NULL; /* Not supported */
    }

    // if (!priv)
    //     return NULL;

    // if (verto_get_flags(ev) & VERTO_EV_FLAG_PRIORITY_HIGH)
    //     event_priority_set(priv, 0);
    // else if (verto_get_flags(ev) & VERTO_EV_FLAG_PRIORITY_MEDIUM)
    //     event_priority_set(priv, 1);
    // else if (verto_get_flags(ev) & VERTO_EV_FLAG_PRIORITY_LOW)
    //     event_priority_set(priv, 2);

    // event_add(priv, timeout);
    return priv;
}

// #define libhv_ctx_new NULL
// #define libhv_ctx_run_once NULL
// #define libhv_ctx_reinitialize NULL
// #define libhv_ctx_set_flags NULL
// VERTO_MODULE(libhv, event_base_init,
//              VERTO_EV_TYPE_IO |
//              VERTO_EV_TYPE_TIMEOUT |
//              VERTO_EV_TYPE_SIGNAL);

// #define VERTO_MODULE(name, symb, types) 
static verto_ctx_funcs libhv_funcs = { 
    NULL, 
    libhv_ctx_default, 
    libhv_ctx_free, 
    libhv_ctx_run, 
    NULL, 
    libhv_ctx_break, 
    NULL, 
    NULL, 
    libhv_ctx_add, 
    libhv_ctx_del 
}; 
verto_module VERTO_MODULE_TABLE(libhv) = { 
    VERTO_MODULE_VERSION, 
    "libhv",
    "event_base_init",
    VERTO_EV_TYPE_IO | VERTO_EV_TYPE_TIMEOUT | VERTO_EV_TYPE_SIGNAL, 
    &libhv_funcs, 
}; 
verto_ctx * 
verto_new_libhv() 
{ 
    return verto_convert(libhv, 0, NULL); 
} 
verto_ctx * 
verto_default_libhv() 
{ 
    return verto_convert(libhv, 1, NULL); 
}

