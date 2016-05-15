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

CT_EXPORT void ct_translate2f(float* ptr, CT_Vec2f* t, size_t num,
                              size_t fstride);
CT_EXPORT void ct_scale2f(float* ptr, CT_Vec2f* t, size_t num, size_t fstride);
CT_EXPORT void ct_translate3f(float* ptr, CT_Vec3f* t, size_t num,
                              size_t fstride);
CT_EXPORT void ct_scale3f(float* ptr, CT_Vec3f* t, size_t num, size_t fstride);
CT_EXPORT void ct_translate4f(float* ptr, CT_Vec4f* t, size_t num,
                              size_t fstride);
CT_EXPORT void ct_scale4f(float* ptr, CT_Vec4f* t, size_t num, size_t fstride);

#ifdef __cplusplus
}
#endif
