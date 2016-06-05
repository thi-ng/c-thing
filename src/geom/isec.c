#include "geom/isec.h"

#define DELTA(a, b, c) ((a) < (b) ? ((a) - (b)) : ((a) > (c) ? ((a) - (c)) : 0))

CT_EXPORT int ct_intersect_rect_circle(const CT_Vec2f *p, const CT_Vec2f *q,
                                       const CT_Vec2f *c, float r) {
  const float dx = DELTA(c->x, p->x, q->x);
  const float dy = DELTA(c->y, p->y, q->y);
  return (dx * dx + dy * dy) <= (r * r);
}

CT_EXPORT int ct_intersect_aabb_sphere(const CT_Vec3f *p, const CT_Vec3f *q,
                                       const CT_Vec3f *c, float r) {
  const float dx = DELTA(c->x, p->x, q->x);
  const float dy = DELTA(c->y, p->y, q->y);
  const float dz = DELTA(c->z, p->z, q->z);
  return (dx * dx + dy * dy + dz * dz) <= (r * r);
}
