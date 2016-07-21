#pragma once

#include "math/vec.h"

CT_BEGIN_DECLS

int ct_intersect_lines(const CT_Vec2f *a,
                       const CT_Vec2f *b,
                       const CT_Vec2f *c,
                       const CT_Vec2f *d,
                       CT_Vec2f *isec,
                       float *alpha,
                       float *beta);

// p/q = rect min/max, c = circle origin, r = radius
int ct_intersect_rect_circle(const CT_Vec2f *p,
                             const CT_Vec2f *q,
                             const CT_Vec2f *c,
                             float r);

int ct_intersect_aabb_sphere(const CT_Vec3f *p,
                             const CT_Vec3f *q,
                             const CT_Vec3f *c,
                             float r);

CT_END_DECLS
