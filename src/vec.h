#pragma once

#include <math.h>
#include <stdint.h>

#include "config.h"
#include "mpool.h"
#include "ct_math.h"

#include "vec2.h"

#ifdef CT_USE_SSE
#include "vec3_sse.h"
#include "vec4_sse.h"
#else
#include "vec3.h"
#include "vec4.h"
#endif

#include "vec3_common.h"
