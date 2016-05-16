#include "vec.h"

CT_EXPORT void ct_translate2f(float* ptr, CT_Vec2f* t, size_t num,
                              size_t fstride);
CT_EXPORT void ct_scale2f(float* ptr, CT_Vec2f* t, size_t num, size_t fstride);
CT_EXPORT void ct_translate3f(float* ptr, CT_Vec3f* t, size_t num,
                              size_t fstride);
CT_EXPORT void ct_scale3f(float* ptr, CT_Vec3f* t, size_t num, size_t fstride);
CT_EXPORT void ct_translate4f(float* ptr, CT_Vec4f* t, size_t num,
                              size_t fstride);
CT_EXPORT void ct_scale4f(float* ptr, CT_Vec4f* t, size_t num, size_t fstride);
