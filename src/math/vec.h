#pragma once

#define IVEC(...) \
  (size_t[]) {    \
    __VA_ARGS__   \
  }
#define FVEC(...) \
  (float[]) {     \
    __VA_ARGS__   \
  }

#include <math.h>
#include <stddef.h>
#include <stdint.h>

#include "cthing.h"
#include "math/hashfn.h"
#include "math/math.h"
#include "mem/mpool.h"

#include "math/vec2.h"

#ifdef CT_FEATURE_SSE
#include "math/vec3_sse.h"
#include "math/vec4_sse.h"
#else
#include "math/vec3.h"
#include "math/vec4.h"
#endif  // CT_FEATURE_SSE

#include "math/vec3_common.h"
#include "math/vec4_common.h"

#include "math/swizzle.h"

#include "math/vec_arrayops.h"
