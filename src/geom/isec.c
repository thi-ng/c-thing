#include "geom/isec.h"

#define DELTA(a, b, c) ((a) < (b) ? ((a) - (b)) : ((a) > (c) ? ((a) - (c)) : 0))

ct_export int ct_intersect_lines_simple(const CT_Vec2f *a,
                                        const CT_Vec2f *b,
                                        const CT_Vec2f *c,
                                        const CT_Vec2f *d,
                                        float *tp) {
  float dabx = b->x - a->x;
  float daby = b->y - a->y;
  float dcdx = d->x - c->x;
  float dcdy = d->y - c->y;
  float det  = dabx * dcdy - daby * dcdx;
  if (fabs(det) < EPS) {
    return 0;
  }
  float dacx = c->x - a->x;
  float dacy = c->y - a->y;
  *tp        = (dacx * dcdy - dacy * dcdx) / det;
  float tq   = (daby * dacx - dabx * dacy) / det;
  return !(*tp < 0 || *tp > 1.0 || tq < 0 || tq > 1.0);
}

ct_export int ct_intersect_lines(const CT_Vec2f *a,
                                 const CT_Vec2f *b,
                                 const CT_Vec2f *c,
                                 const CT_Vec2f *d,
                                 float *alpha,
                                 float *beta,
                                 CT_Vec2f *isec) {
  float tp;
  if (ct_intersect_lines_simple(a, b, c, d, &tp)) {
    ct_mix2fv(a, b, tp, isec);
    *alpha = ct_dist2fv(a, isec) / ct_dist2fv(a, b);
    *beta  = ct_dist2fv(c, isec) / ct_dist2fv(c, d);
    return 1;
  }
  return 0;
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
