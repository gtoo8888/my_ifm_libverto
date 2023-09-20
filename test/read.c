#include <stdint.h>
#include <signal.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "ifm_verto.h"


#define DATA "hello"
#define DATALEN 5

static int fds[2];
static int callcount = 0;

static void
timeout_cb(verto_ctx *ctx, verto_ev *ev)
{
    (void) ev;

    printf("ERROR: Timeout!\n");
    if (fds[0] >= 0)
        close(fds[0]);
    if (fds[1] >= 0)
        close(fds[1]);

    retval = 1;
    verto_break(ctx);
}

static void
cb(verto_ctx *ctx, verto_ev *ev)
{
    unsigned char buff[DATALEN];
    int fd = 0;
    ssize_t bytes = 0;

    fd = verto_get_fd(ev);
    assert(fd == fds[0]);

    bytes = read(fd, buff, DATALEN);
    if (callcount++ == 0) {
        printf("buff:%s\n",buff);
        assert(verto_get_fd_state(ev) & VERTO_EV_FLAG_IO_READ);
        assert(bytes == DATALEN);
        close(fds[1]);
        fds[1] = -1;
    }
    else {
        if (!(verto_get_fd_state(ev) & VERTO_EV_FLAG_IO_ERROR))
            printf("WARNING: VERTO_EV_FLAG_IO_ERROR not supported!\n");
        assert(bytes != DATALEN);
        close(fd);
        fds[0] = -1;
        verto_del(ev);
        verto_break(ctx);
    }
}

int
do_test_read(verto_ctx *ctx)
{
    callcount = 0;
    fds[0] = -1;
    fds[1] = -1;

    assert(verto_get_supported_types(ctx) & VERTO_EV_TYPE_IO);

    assert(pipe(fds) == 0);
    // assert(verto_add_timeout(ctx, VERTO_EV_FLAG_NONE, timeout_cb, 1000));
    assert(verto_add_io(ctx, VERTO_EV_FLAG_PERSIST | VERTO_EV_FLAG_IO_READ, cb, fds[0]));
    assert(write(fds[1], DATA, DATALEN) == DATALEN);
    return 0;
}
