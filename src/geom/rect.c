#include "geom/rect.h"

CT_EXPORT float ct_rect2f_area(CT_Rect2f *r) {
  return (r->q.x - r->p.x) * (r->q.y - r->p.y);
}

CT_EXPORT int ct_rect2f_classify_point(CT_Rect2f *r, CT_Vec2f *p) {
  if (p->x > r->p.x && p->x < r->q.x && p->y > r->p.y && p->y < r->q.y) {
    return 1;
  } else if (p->x < r->p.x || p->x > r->q.x || p->y < r->p.y || p->y > r->q.y) {
    return -1;
  }
  return 0; // FIXME
}
