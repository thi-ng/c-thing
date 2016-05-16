#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stddef.h>
#include <stdint.h>

#include "config.h"
#include "ct_math.h"
#include "mpool.h"

#include "vec2.h"

#ifdef CT_FEATURE_SSE
#include "vec3_sse.h"
#include "vec4_sse.h"
#else
#include "vec3.h"
#include "vec4.h"
#endif

#include "vec3_common.h"
#include "vec4_common.h"

#include "swizzle.h"

#include "vec_arrayops.h"

#ifdef __cplusplus
}
#endif
