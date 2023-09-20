#include <stdio.h>
#include "ifm_verto.h"


static char* modules_name = "libhv";


int main(int argc,char **argv){
    printf("%s\n","ifm_verto");

    verto_ctx *ctx;

    ctx = verto_default(modules_name, VERTO_EV_TYPE_NONE);

    retval = do_test_read(ctx);
    // retval = do_test_timeout2(ctx);
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
