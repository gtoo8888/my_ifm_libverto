
#include "ifm_verto_libhv.h"
#define VERTO_MODULE_TYPES
typedef struct hloop_t verto_mod_ctx;
typedef struct hevent_t verto_mod_ev;
#include "ifm_verto_module.h"

#include <hv/hloop.h>

static char host[64] = "127.0.0.1";
static int  port = 80;

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


static void libhv_timer(htimer_t* timer) {
    printf("time=%lus\n",(unsigned long)time(NULL));
}


static void on_write(hio_t* io, const void* buf, int writebytes) {
    // if (!io) return;
    // if (!hio_write_is_complete(io)) return;
    // http_conn_t* conn = (http_conn_t*)hevent_userdata(io);
    // http_send_file(conn);
}


static void
libhv_callback_io_write(hio_t* io, const void* buf, int writebytes)
{

}

static void
libhv_callback_io_read(hio_t* io, void* buf, int readbytes)
{

}

// static void
// libevent_callback(evutil_socket_t socket, short type, void *data)
static void
libhv_callback_io(hio_t* io)
{
    verto_ev_flag state = VERTO_EV_FLAG_NONE;

    void *data;

    (void) socket;
    
    // if (io->events & HV_READ)
    //     state |= VERTO_EV_FLAG_IO_READ;
    // if (io->events & HV_WRITE)
    //     state |= VERTO_EV_FLAG_IO_WRITE;
    verto_set_fd_state(data, state);
    verto_fire(data);
}

// typedef void (*hread_cb)    (hio_t* io, void* buf, int readbytes);
// typedef void (*hwrite_cb)   (hio_t* io, const void* buf, int writebytes);

static void libhv_callback_signal(void* userdata) {
    // hlogi("reload confile [%s]", g_main_ctx.confile);
    // parse_confile(g_main_ctx.confile);
}

static verto_mod_ev *
libhv_ctx_add(verto_mod_ctx *ctx, const verto_ev *ev, verto_ev_flag *flags)
{
    struct verto_mod_ev *priv = NULL;
    struct timeval *timeout = NULL;
    struct timeval tv;
    int libhvflags = 0;

    // *flags |= verto_get_flags(ev) & VERTO_EV_FLAG_PERSIST;
    // if (verto_get_flags(ev) & VERTO_EV_FLAG_PERSIST)
    //     libeventflags |= EV_PERSIST;
    hio_t* io = hio_create_socket(ctx, host, port, HIO_TYPE_TCP, HIO_CLIENT_SIDE);

    switch (verto_get_type(ev)) {
    case VERTO_EV_TYPE_IO:
        // hio_t hio = hio_create(loop, fd, EV_READ, embed_io_cb, NULL);
        if (verto_get_flags(ev) & VERTO_EV_FLAG_IO_READ){
            libhvflags |= HV_READ;
            hio_add(io, libhv_callback_io, HV_READ);
            hio_setcb_read(io, libhv_callback_io_read);
        }
        if (verto_get_flags(ev) & VERTO_EV_FLAG_IO_WRITE){
            libhvflags |= HV_WRITE;
            hio_add(io, libhv_callback_io, HV_WRITE);
            hio_setcb_write(io, libhv_callback_io_write);
        }
        // io = hloop_create_udp_client(loop, host, port);

        // // setuptype(io, libev_callback, verto_get_fd(ev), EV_NONE);
        // // #define setuptype(io, ...) 
        // w.io = malloc(sizeof(ev_io)); 
        // if (w.io) { 
        //     ev_io_init(w.io, (EV_CB(type, (*))) __VA_ARGS__); 
        //     ev_io_start(ctx, w.io); 
        // } 
        // break

        // hio_add(io, hio_handle_events, HV_READ);
        // hio_add(io, hio_handle_events, HV_READ);  
        // hio_setcb_write(io, on_write);
        // hio_setcb_close(io, on_close);
        // hio_setcb_read(io, on_recv);
    
        // priv = event_new(ctx, verto_get_fd(ev), libeventflags,
        //                  libevent_callback, (void *) ev);
        break;
    case VERTO_EV_TYPE_TIMEOUT:
        timeout = &tv;
        time_t tmp = verto_get_interval(ev);
        time_t tmp1 = tmp/1000;
        tv.tv_sec = verto_get_interval(ev) / 1000;
        // tv.tv_usec = verto_get_interval(ev) % 1000 * 1000;
        // uint32_t milliseconds = tv.tv_sec * 1000LL;
        uint32_t milliseconds = 10;

        htimer_add(ctx, libhv_timer, milliseconds, INFINITE);

        // priv = event_new(ctx, -1, EV_TIMEOUT | libeventflags,
        //                  libevent_callback, (void *) ev);
        break;
    case VERTO_EV_TYPE_SIGNAL:
        signal_init(libhv_callback_signal);
        // const char* signal = get_arg("s");
        // if (signal) {
        signal_handle(signal);
        // }
        // libhvflags = EV_SIGNAL | libhvflags,
        // ev = verto_add_signal(gpctx->vctx, VERTO_EV_FLAG_PERSIST,
        //                       hup_handler, SIGHUP);
        // ev->option.signal = signal
        // ev->ev = ctx->module->funcs->ctx_add(ctx->ctx, ev, &ev->actual); \    

        // signal(SIGPIPE, SIG_IGN);

        // priv = event_new(ctx, verto_get_signal(ev),
        //                  EV_SIGNAL | libeventflags,
        //                  libevent_callback, (void *) ev);
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
