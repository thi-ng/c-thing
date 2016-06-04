#include "geom/isec.h"

CT_EXPORT int ct_intersect_rect_circle(const CT_Vec2f *p, const CT_Vec2f *q,
                                       const CT_Vec2f *c, float r) {
  const float dx =
      (c->x < p->x ? (c->x - p->x) : (c->x > q->x ? (c->x - q->x) : 0.0f));
  const float dy =
      (c->y < p->y ? (c->y - p->y) : (c->y > q->y ? (c->y - q->y) : 0.0f));
  return (dx * dx + dy * dy) <= (r * r);
}
