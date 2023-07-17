#include "test.h"

// void do_test(verto_ctx *ctx){
//     printf("write.c\n");
// }

std::vector<std::string> module = {
    "glib",
    "libev",
    "libevent",
};

int main(int argc,char **argv){
    printf("ok !\n");

    verto_ctx *ctx;
    std::cout << ctx << std::endl;
    printf("verto_ctx !\n");

    // assert

    for (int i = 0;i < module.size(); i++){
        std::cout << module[i] << std::endl;
    }

    ctx = verto_default(NULL, VERTO_EV_TYPE_NONE);

    do_test(ctx);
    
    return 0;
}