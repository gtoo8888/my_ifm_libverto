#include "ifm_libverto_test.h"
#include "ifm_verto.h"

#include <event2/event_compat.h>

static verto_mod_ctx *
libevent_ctx_new(void)
{
    verto_mod_ctx* eb;
    return eb;
}

static verto_mod_ctx *
libevent_ctx_default(void)
{
    verto_mod_ctx* eb;
    return eb;
}

static void
libevent_ctx_free(verto_mod_ctx *ctx)
{
    
}

static void
libevent_ctx_run(verto_mod_ctx *ctx)
{
    
}

static void
libevent_ctx_run_once(verto_mod_ctx *ctx)
{
   
}

static void
libevent_ctx_break(verto_mod_ctx *ctx)
{
    
}

static void
libevent_ctx_reinitialize(verto_mod_ctx *ctx)
{
    
}

static void
libevent_callback(evutil_socket_t socket, short type, void *data)
{
    
}

static verto_mod_ev *
libevent_ctx_add(verto_mod_ctx *ctx, const verto_ev *ev, verto_ev_flag *flags)
{
    verto_mod_ctx* eb;
    return eb;
}

static void
libevent_ctx_del(verto_mod_ctx *ctx, const verto_ev *ev, verto_mod_ev *evpriv)
{
    
}





void test_covert(void){
    std::cout << "test_covert" << std::endl;

    std::string name = "libevent-test";

    std::cout << "VERTO_MODULE_TABLE(name)" << std::endl;

    // #define libevent_ctx_set_flags NULL
    // VERTO_MODULE(libevent, event_base_init, static_cast<verto_ev_type>(
    //          VERTO_EV_TYPE_IO |
    //          VERTO_EV_TYPE_TIMEOUT |
    //          VERTO_EV_TYPE_SIGNAL));
}