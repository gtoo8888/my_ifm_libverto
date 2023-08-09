#ifndef IFM_VERTO_LIBHV_H_
#define IFM_VERTO_LIBHV_H_

#include "ifm_verto.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

verto_ctx *
verto_new_libhv(void);

verto_ctx *
verto_default_libhv(void);

// verto_ctx *
// verto_convert_libhv(struct event_base *ebase);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* IFM_VERTO_LIBHV_H_ */
