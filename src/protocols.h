#pragma once

#include "aabb.h"
#include "circle.h"
#include "config.h"
#include "rect.h"

#define PROTO_FN1(proto, rtype, name)                            \
  CT_EXPORT ct_inline rtype ct_##name(const proto *p, void *i) { \
    return p->name(i);                                           \
  }

#define PROTO_FN2(proto, rtype, name, atype)                               \
  CT_EXPORT ct_inline rtype ct_##name(const proto *p, void *i, atype *a) { \
    return p->name(i, a);                                                  \
  }

CT_BEGIN_DECLS

typedef struct { float (*area)(void *); } CT_IArea;

typedef struct {
  CT_Rect2f (*bounds2f)(void *);
  CT_AABB (*bounds3f)(void *);
} CT_IBounds;

typedef struct { float (*circumference)(void *); } CT_ICircumference;

typedef struct {
  int (*classify_point2f)(void *, CT_Vec2f *);
  int (*classify_point3f)(void *, CT_Vec3f *);
} CT_IClassify;

PROTO_FN1(CT_IArea, float, area)

PROTO_FN1(CT_IBounds, CT_Rect2f, bounds2f)
PROTO_FN1(CT_IBounds, CT_AABB, bounds3f)

PROTO_FN1(CT_ICircumference, float, circumference)

PROTO_FN2(CT_IClassify, int, classify_point2f, CT_Vec2f)
PROTO_FN2(CT_IClassify, int, classify_point3f, CT_Vec3f)

CT_END_DECLS
