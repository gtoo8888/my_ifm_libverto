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
error_cb(verto_ctx *ctx, verto_ev *ev)
{
    int fd = 0;

    /* When we get here, the fd should be closed, so an error should occur */
    fd = verto_get_fd(ev);
    if (!(verto_get_fd_state(ev) & VERTO_EV_FLAG_IO_ERROR))
        printf("WARNING: VERTO_EV_FLAG_IO_ERROR not supported!\n");
    assert(write(fd, DATA, DATALEN) != DATALEN);
    close(fd);
    fds[1] = -1;
    verto_break(ctx);
}

static void
read_cb(verto_ctx *ctx, verto_ev *ev)
{
    unsigned char buff[DATALEN];
    int fd = verto_get_fd(ev);

    assert(read(fd, buff, DATALEN) == DATALEN);
    close(fd);
    fds[0] = -1;

    assert(verto_add_io(ctx, VERTO_EV_FLAG_IO_WRITE, error_cb, fds[1]));
}

static void
write_cb(verto_ctx *ctx, verto_ev *ev)
{
    int fd = 0;

    fd = verto_get_fd(ev);
    assert(verto_get_fd_state(ev) & VERTO_EV_FLAG_IO_WRITE);
    assert(write(fd, DATA, DATALEN) == DATALEN);

    assert(verto_add_io(ctx, VERTO_EV_FLAG_IO_READ, read_cb, fds[0]));
}

int
do_test_write(verto_ctx *ctx)
{
    callcount = 0;
    fds[0] = -1;
    fds[1] = -1;

    assert(verto_get_supported_types(ctx) & VERTO_EV_TYPE_IO);

    if (!verto_add_signal(ctx, VERTO_EV_FLAG_NONE, VERTO_SIG_IGN, SIGPIPE))
        signal(SIGPIPE, SIG_IGN);

    assert(pipe(fds) == 0);
    assert(verto_add_timeout(ctx, VERTO_EV_FLAG_NONE, timeout_cb, 1000));
    assert(verto_add_io(ctx, VERTO_EV_FLAG_IO_WRITE, write_cb, fds[1]));
    return 0;
}