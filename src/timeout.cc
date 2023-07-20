#include "ifm_libverto_test.h"

#include <sys/time.h>

#define SLEEP 10

static int callcount = 0;
struct timeval starttime;


static void cb(verto_ctx* ctx,verto_ev *ev){

}

int do_test(verto_ctx *ctx){
    printf("write.c\n");

    // int* tmp = nullptr;
    // *tmp = 10;

    callcount = 0;

    assert(gettimeofday(&starttime, NULL) == 0);
    assert(verto_add_timeout(ctx, VERTO_EV_FLAG_NONE, cb, SLEEP));
    return 0;
}





