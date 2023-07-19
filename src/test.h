#ifndef __TEST_H__
#define __TEST_H__

#include <iostream>
#include <vector>
#include <assert.h>

#include <verto.h>



#define UNUSED  __attribute__((unused)) // 不应该未被使用

#define GPDEBUG(...) do { \
    if(1){ \
        fprintf(stderr, __VA_ARGS__); \
    } \
} while(0)

int do_test(verto_ctx *ctx);

extern std::vector<std::string> module;

#endif // __TEST_H__

