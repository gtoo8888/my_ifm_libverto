
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


// int main() {
//     time_t current_time;
//     struct timespec spec;
//     clock_gettime(CLOCK_REALTIME, &spec);
//     current_time = spec.tv_sec;

//     // 将time_t转换为毫秒
//     long long milliseconds = current_time * 1000LL;

//     printf("Current time in milliseconds: %lld\n", milliseconds);

//     return 0;
// }

static void libhv_timer(htimer_t* timer) {
    printf("time=%lus\n",(unsigned long)time(NULL));
}


static verto_mod_ev *
libhv_ctx_add(verto_mod_ctx *ctx, const verto_ev *ev, verto_ev_flag *flags)
{
    struct event *priv = NULL;
    struct timeval *timeout = NULL;
    struct timeval tv;
    int libeventflags = 0;

    // *flags |= verto_get_flags(ev) & VERTO_EV_FLAG_PERSIST;
    // if (verto_get_flags(ev) & VERTO_EV_FLAG_PERSIST)
    //     libeventflags |= EV_PERSIST;

    switch (verto_get_type(ev)) {
    // case VERTO_EV_TYPE_IO:
    //     if (verto_get_flags(ev) & VERTO_EV_FLAG_IO_READ)
    //         libeventflags |= EV_READ;
    //     if (verto_get_flags(ev) & VERTO_EV_FLAG_IO_WRITE)
    //         libeventflags |= EV_WRITE;
    //     priv = event_new(ctx, verto_get_fd(ev), libeventflags,
    //                      libevent_callback, (void *) ev);
    //     break;
    case VERTO_EV_TYPE_TIMEOUT:
        timeout = &tv;
        time_t tmp = verto_get_interval(ev);
        time_t tmp1 = tmp/1000;
        tv.tv_sec = verto_get_interval(ev) / 1000;
        // tv.tv_usec = verto_get_interval(ev) % 1000 * 1000;
        uint32_t milliseconds = tv.tv_sec * 1000LL;

        htimer_add(ctx, libhv_timer, milliseconds, INFINITE);

        // priv = event_new(ctx, -1, EV_TIMEOUT | libeventflags,
        //                  libevent_callback, (void *) ev);
        break;
    // case VERTO_EV_TYPE_SIGNAL:
    //     priv = event_new(ctx, verto_get_signal(ev),
    //                      EV_SIGNAL | libeventflags,
    //                      libevent_callback, (void *) ev);
    //     break;
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
    // return priv;
}

#define libhv_ctx_new NULL
#define libhv_ctx_run_once NULL
#define libhv_ctx_reinitialize NULL
#define libhv_ctx_set_flags NULL
VERTO_MODULE(libhv, event_base_init,
             VERTO_EV_TYPE_IO |
             VERTO_EV_TYPE_TIMEOUT |
             VERTO_EV_TYPE_SIGNAL);
