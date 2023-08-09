#ifndef _IFM_VERTO_H_
#define _IFM_VERTO_H_

#include <stdlib.h>

#define BUILTIN_MODULE libhv

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct verto_ctx verto_ctx;
typedef struct verto_ev verto_ev;


typedef enum {
    VERTO_EV_TYPE_NONE = 0,
    VERTO_EV_TYPE_IO = 1,
    VERTO_EV_TYPE_TIMEOUT = 1 << 1,
    VERTO_EV_TYPE_IDLE = 1 << 2,
    VERTO_EV_TYPE_SIGNAL = 1 << 3,
    VERTO_EV_TYPE_CHILD = 1 << 4
} verto_ev_type;

typedef enum {
    VERTO_EV_FLAG_NONE = 0,
    VERTO_EV_FLAG_PERSIST = 1,
    VERTO_EV_FLAG_PRIORITY_LOW = 1 << 1,
    VERTO_EV_FLAG_PRIORITY_MEDIUM = 1 << 2,
    VERTO_EV_FLAG_PRIORITY_HIGH = 1 << 3,
    VERTO_EV_FLAG_IO_READ = 1 << 4,
    VERTO_EV_FLAG_IO_WRITE = 1 << 5,
    VERTO_EV_FLAG_IO_ERROR = 1 << 7,
    VERTO_EV_FLAG_IO_CLOSE_FD = 1 << 8,
    VERTO_EV_FLAG_REINITIABLE = 1 << 6,
    _VERTO_EV_FLAG_MUTABLE_MASK = VERTO_EV_FLAG_PRIORITY_LOW
                                  | VERTO_EV_FLAG_PRIORITY_MEDIUM
                                  | VERTO_EV_FLAG_PRIORITY_HIGH
                                  | VERTO_EV_FLAG_IO_READ
                                  | VERTO_EV_FLAG_IO_WRITE,
    _VERTO_EV_FLAG_MAX = VERTO_EV_FLAG_IO_CLOSE_FD
} verto_ev_flag;



verto_ctx *
verto_default(const char *impl, verto_ev_type reqtypes);

void
verto_run(verto_ctx *ctx);

void
verto_break(verto_ctx *ctx);

void
verto_free(verto_ctx *ctx);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif