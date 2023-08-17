#include <stdint.h>
#include <signal.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "ifm_verto.h"
static int count;

void
exit_cb(verto_ctx *ctx, verto_ev *ev)
{
    if ((pid_t) (uintptr_t) verto_get_private(ev) != 0)
        waitpid((pid_t) (uintptr_t) verto_get_private(ev), NULL, 0);

    switch (count) {
        case 0:
            printf("ERROR: Signal callback never fired!\n");
            break;
        case 1:
            printf("ERROR: Signal MUST recur!\n");
            break;
        default:
            break;
    }
    retval = count != 2;
    verto_break(ctx);
}

void
cb(verto_ctx *ctx, verto_ev *ev)
{
    (void) ctx;
    (void) ev;

    count++;
}

int
do_test_signal(verto_ctx *ctx)
{
    verto_ev *ev;
    pid_t pid = 0;
    count = 0;

    if (!(verto_get_supported_types(ctx) & VERTO_EV_TYPE_SIGNAL)) {
        printf("WARNING: Signal not supported!\n");
        verto_break(ctx);
        return 0;
    }

    /* We should get a failure when trying to create a non-persistent ignore */
    assert(!verto_add_signal(ctx, VERTO_EV_FLAG_NONE, VERTO_SIG_IGN, SIGUSR2));
    assert(verto_add_signal(ctx, VERTO_EV_FLAG_PERSIST, cb, SIGUSR1));
    assert(verto_add_signal(ctx, VERTO_EV_FLAG_PERSIST, VERTO_SIG_IGN, SIGUSR2));

    pid = fork();
    if (pid < 0)
        return 1;
    else if (pid == 0) {
        usleep(10000); /* 0.01 seconds */
        kill(getppid(), SIGUSR1);
        usleep(10000); /* 0.01 seconds */
        kill(getppid(), SIGUSR1);
        usleep(10000); /* 0.01 seconds */
        kill(getppid(), SIGUSR2);
        exit(0);
    }

    ev = verto_add_timeout(ctx, VERTO_EV_FLAG_NONE, exit_cb, 1000);
    assert(ev);
    verto_set_private(ev, (void *) (uintptr_t) pid, NULL);
    return 0;
}