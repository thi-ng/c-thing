#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stddef.h>
#include <stdint.h>

#include "config.h"
#include "math/math.h"
#include "math/hashfn.h"
#include "mem/mpool.h"

#include "math/vec2.h"

#ifdef CT_FEATURE_SSE
#include "math/vec3_sse.h"
#include "math/vec4_sse.h"
#else
#include "math/vec3.h"
#include "math/vec4.h"
#endif

#include "math/vec3_common.h"
#include "math/vec4_common.h"

#include "math/swizzle.h"

#include "math/vec_arrayops.h"

#ifdef __cplusplus
}
#endif
