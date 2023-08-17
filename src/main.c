#include <stdio.h>
#include "ifm_verto.h"

static int retval = 0;

static char* modules_name = "libhv";

int
do_test(verto_ctx *ctx)
{
    verto_ev *ev;
    pid_t pid = 0;
    // count = 0;

    // if (!(verto_get_supported_types(ctx) & VERTO_EV_TYPE_SIGNAL)) {
    //     printf("WARNING: Signal not supported!\n");
    //     verto_break(ctx);
    //     return 0;
    // }

    /* We should get a failure when trying to create a non-persistent ignore */
    // assert(!verto_add_signal(ctx, VERTO_EV_FLAG_NONE, VERTO_SIG_IGN, SIGUSR2));
    // assert(verto_add_signal(ctx, VERTO_EV_FLAG_PERSIST, cb, SIGUSR1));
    // assert(verto_add_signal(ctx, VERTO_EV_FLAG_PERSIST, VERTO_SIG_IGN, SIGUSR2));

    // pid = fork();
    // if (pid < 0)
    //     return 1;
    // else if (pid == 0) {
    //     usleep(10000); /* 0.01 seconds */
    //     kill(getppid(), SIGUSR1);
    //     usleep(10000); /* 0.01 seconds */
    //     kill(getppid(), SIGUSR1);
    //     usleep(10000); /* 0.01 seconds */
    //     kill(getppid(), SIGUSR2);
    //     exit(0);
    // }

    // ev = verto_add_timeout(ctx, VERTO_EV_FLAG_NONE, exit_cb, 1000);
    // assert(ev);
    // verto_set_private(ev, (void *) (uintptr_t) pid, NULL);
    return 0;
}


int main(int argc,char **argv){
    printf("%s\n","ifm_verto");

    verto_ctx *ctx;

    ctx = verto_default(modules_name, VERTO_EV_TYPE_NONE);

    retval = do_test_timeout(ctx);
    if (retval != 0) {
        verto_free(ctx);
        return retval;
    }

    verto_run(ctx);
    verto_free(ctx);

    if (retval != 0)
        return retval;

    verto_cleanup();

    return retval;
}
