#include "ifm_verto.h"
#include <sys/time.h>
#include <assert.h>

#define SLEEP 10
#define SLEEP_MIN (SLEEP-10)
#define SLEEP_MAX (SLEEP*4)
#define M2U(m) ((m) * 1000)
static int callcount;
struct timeval starttime;

static char
elapsed(time_t min, time_t max)
{
    struct timeval tv;
    long long diff;

    assert(gettimeofday(&tv, NULL) == 0);
    diff = (tv.tv_sec - starttime.tv_sec) * M2U(1000)
            + tv.tv_usec - starttime.tv_usec;

    assert(gettimeofday(&starttime, NULL) == 0);
    if (diff < M2U(min) || diff > M2U(max)) {
        printf("ERROR: Timeout is out-of-bounds!\n");
        return 0;
    }
    return 1;
}

static void
exit_cb(verto_ctx *ctx, verto_ev *ev)
{
    (void) ev;
    assert(callcount == 3);
    verto_break(ctx);
}

static void
cb(verto_ctx *ctx, verto_ev *ev)
{
    assert(elapsed(SLEEP_MIN, SLEEP_MAX));
    if (++callcount == 3)
        assert(verto_add_timeout(ctx, VERTO_EV_FLAG_NONE, exit_cb, SLEEP*2));
    else if (callcount == 2) {
        assert(verto_add_timeout(ctx, VERTO_EV_FLAG_NONE, cb, SLEEP));
        verto_del(ev);
    }
}

int
do_test_timeout(verto_ctx *ctx)
{
    callcount = 0;

    assert(gettimeofday(&starttime, NULL) == 0);
    assert(verto_add_timeout(ctx, VERTO_EV_FLAG_PERSIST, cb, SLEEP));
    return 0;
}