#include "geom/rect.h"

ct_export float ct_rect2f_area(const CT_Rect2f *r) {
  return (r->q.x - r->p.x) * (r->q.y - r->p.y);
}

ct_export int ct_rect2f_classify_point(const CT_Rect2f *r, const CT_Vec2f *p) {
  if (p->x > r->p.x && p->x < r->q.x && p->y > r->p.y && p->y < r->q.y) {
    return 1;
  } else if (p->x < r->p.x || p->x > r->q.x || p->y < r->p.y || p->y > r->q.y) {
    return -1;
  }
  return 0;  // FIXME
}

ct_export CT_Vec2f *ct_rect2f_sample_inside(const CT_Rect2f *r, CT_Vec2f *out) {
  out->x = ct_rand_normpos() * (r->q.x - r->p.x) + r->p.x;
  out->y = ct_rand_normpos() * (r->q.y - r->p.y) + r->p.y;
  return out;
}
