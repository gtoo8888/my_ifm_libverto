#include "ifm_libverto_test.h"
#include "ifm_verto.h"
#include <signal.h>

std::vector<std::string> module = {
    "glib",
    "libev",
    "libevent",
};






struct gssproxy_ctx {
    bool userproxymode;
    char *config_file;
    char *config_dir;
    char *config_socket;
    int daemonize;

    struct gp_config *config;
    struct gp_workers *workers;
    verto_ctx *vctx;
    verto_ev *sock_ev;      /* default socket event */

    time_t term_timeout;
    verto_ev *term_ev; /* termination ev in user mode */

    ssize_t readstats;
    ssize_t writestats;
    time_t last_activity;
};

static int retval = 0;


static void break_loop(verto_ctx *vctx,verto_ev* ev UNUSED){
    GPDEBUG("Exiting after receiving a signal\n");
    verto_break(vctx);
}

static void hup_handler(verto_ctx *vctx UNUSED, verto_ev *ev){

}

void init_event_loop(){
    verto_ctx *gp_ctx;
    verto_ev *gp_ev;
    // gp_ctx = verto_default(NULL, VERTO_EV_TYPE_IO | VERTO_EV_TYPE_SIGNAL | VERTO_EV_TYPE_TIMEOUT);
    gp_ctx = verto_default(NULL, (verto_ev_type)(VERTO_EV_TYPE_IO | VERTO_EV_TYPE_SIGNAL | VERTO_EV_TYPE_TIMEOUT));
    if(!gp_ctx){
        goto fail;
    }

    gp_ev = verto_add_signal(gp_ctx, VERTO_EV_FLAG_PERSIST, break_loop, SIGINT);
    gp_ev = verto_add_signal(gp_ctx, VERTO_EV_FLAG_PERSIST, break_loop, SIGTERM);
    gp_ev = verto_add_signal(gp_ctx, VERTO_EV_FLAG_PERSIST, break_loop, SIGQUIT);
    gp_ev = verto_add_signal(gp_ctx, VERTO_EV_FLAG_PERSIST, VERTO_SIG_IGN, SIGPIPE);
    gp_ev = verto_add_signal(gp_ctx, VERTO_EV_FLAG_PERSIST, VERTO_SIG_IGN, SIGHUP);
    gp_ev = verto_add_signal(gp_ctx, VERTO_EV_FLAG_PERSIST, hup_handler, SIGHUP);
    if (gp_ev) 
        verto_set_private(gp_ev, gp_ctx, NULL);

    return;

fail:
    if (gp_ctx) {
        verto_free(gp_ctx);
        gp_ctx = NULL;
    }
}

// 超时后终止gssproxy进程
static void idle_terminate(verto_ctx *vctx, verto_ev *ev){
    // 从事件对象ev中获取私有数据gpctx
    struct gssproxy_ctx *gpctx = static_cast<struct gssproxy_ctx *>(verto_get_private(ev));

    verto_break(vctx);
}

// 事件处理函数，用于在gssproxy空闲时启动一个超时处理器
void idle_handler(struct gssproxy_ctx *gp_ctx){
    verto_ev* gp_ev;
    // 删除之前的超时处理器，然后添加一个新的超时处理器。
    verto_del(gp_ev);

    time_t term_timeout = 1;
    gp_ev = verto_add_timeout(gp_ctx->vctx, VERTO_EV_FLAG_NONE,
                                       idle_terminate, term_timeout);
    if (!gp_ev) {
        GPDEBUG("Failed to register timeout event!\n");
    }
    verto_set_private(gp_ev, gp_ctx, NULL);
}

static void delayed_init(verto_ctx *vctx UNUSED, verto_ev *ev){
    struct gssproxy_ctx *gpctx = static_cast<struct gssproxy_ctx *>(verto_get_private(ev));
    idle_handler(gpctx);
}

// 初始化事件处理
int init_event_fini(verto_ctx *vctx){
    verto_ev *ev;

    ev = verto_add_timeout(vctx, VERTO_EV_FLAG_NONE, delayed_init, 1); // 报错
    if (!ev) {
        fprintf(stderr, "Failed to register delayed_init event!\n");
        return EXIT_FAILURE;
    }
    // verto_set_private(ev, gpctx, NULL);

    return 0;
}



int main(int argc,char **argv){
    verto_ctx *ctx;

    test_covert();



    init_event_loop();


    // init_event_fini(ctx);

    // for (int i = 0;i < module.size(); i++){
    //     std::cout << module[i] << std::endl;
    // }

    ctx = verto_default(module[0].c_str(), VERTO_EV_TYPE_NONE);

    retval = do_test(ctx);
    return retval;

    if (retval != 0){
        verto_free(ctx);
        return retval;
    }
    verto_run(ctx);
    verto_free(ctx);
    
    if(retval != 0)
        return retval;
    
    verto_cleanup();
    
    return retval;
}