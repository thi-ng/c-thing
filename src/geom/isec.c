#include "geom/isec.h"

#define DELTA(a, b, c) ((a) < (b) ? ((a) - (b)) : ((a) > (c) ? ((a) - (c)) : 0))

// returns: -1 = parallel, 0 = isec outside, 1 = isec inside
ct_export int ct_intersect_lines(const CT_Vec2f *a,
                                 const CT_Vec2f *b,
                                 const CT_Vec2f *c,
                                 const CT_Vec2f *d,
                                 CT_Vec2f *isec,
                                 float *alpha,
                                 float *beta) {
  float bax = b->x - a->x;
  float bay = b->y - a->y;
  float dcx = d->x - c->x;
  float dcy = d->y - c->y;
  float det = (dcy * bax - dcx * bay);
  if (det == 0) {
    return -1;
  }
  float acx  = a->x - c->x;
  float acy  = a->y - c->y;
  *alpha     = (dcx * acy - dcy * acx) / det;
  *beta      = (bax * acy - bay * acx) / det;
  int inside = (0 < *alpha && *alpha < 1) && (0 < *beta && *beta < 1);
  if (inside) {
    ct_mix2fv(a, b, *alpha, isec);
  }
  return inside;
}

ct_export int ct_intersect_rect_circle(const CT_Vec2f *p,
                                       const CT_Vec2f *q,
                                       const CT_Vec2f *c,
                                       float r) {
  const float dx = DELTA(c->x, p->x, q->x);
  const float dy = DELTA(c->y, p->y, q->y);
  return (dx * dx + dy * dy) <= (r * r);
}

ct_export int ct_intersect_aabb_sphere(const CT_Vec3f *p,
                                       const CT_Vec3f *q,
                                       const CT_Vec3f *c,
                                       float r) {
  const float dx = DELTA(c->x, p->x, q->x);
  const float dy = DELTA(c->y, p->y, q->y);
  const float dz = DELTA(c->z, p->z, q->z);
  return (dx * dx + dy * dy + dz * dz) <= (r * r);
}
