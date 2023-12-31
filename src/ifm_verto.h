#ifndef _IFM_VERTO_H_
#define _IFM_VERTO_H_

#include <stdlib.h>

#define BUILTIN_MODULE libhv

#define VERTO_SIG_IGN ((verto_callback *) 1)

static int retval = 0;

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

// 暂时不明
typedef void (verto_callback)(verto_ctx *ctx, verto_ev *ev);


verto_ctx *
verto_default(const char *impl, verto_ev_type reqtypes);

void
verto_run(verto_ctx *ctx);

void
verto_break(verto_ctx *ctx);

void
verto_del(verto_ev *ev);

void
verto_free(verto_ctx *ctx);

void
verto_cleanup(void);

void *
verto_get_private(const verto_ev *ev);

verto_ev *
verto_add_signal(verto_ctx *ctx, verto_ev_flag flags,
                 verto_callback *callback, int signal);

verto_ev *
verto_add_io(verto_ctx *ctx, verto_ev_flag flags,
             verto_callback *callback, int fd);

verto_ev *
verto_add_timeout(verto_ctx *ctx, verto_ev_flag flags,
                  verto_callback *callback, time_t interval);

int
verto_get_fd(const verto_ev *ev);

verto_ev_type
verto_get_type(const verto_ev *ev);

void *
verto_get_private(const verto_ev *ev);

// 额外添加的
time_t
verto_get_interval(const verto_ev *ev);

verto_ev_flag
verto_get_fd_state(const verto_ev *ev);

verto_ev_type
verto_get_supported_types(verto_ctx *ctx);

verto_ev_flag
verto_get_flags(const verto_ev *ev);

verto_ev_type
verto_get_type(const verto_ev *ev);

void
verto_set_private(verto_ev *ev, void *priv, verto_callback *free);

void
verto_fire(verto_ev *ev);

void
verto_set_fd_state(verto_ev *ev, verto_ev_flag state);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif