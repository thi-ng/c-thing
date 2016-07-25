#include "geom/hull.h"

static size_t hull2f(CT_Vec2f *points, size_t num, CT_Vector *h) {
  size_t len = 0;
  for (size_t i = 0; i < num; i++) {
    while (len >= 2 &&
           ct_cross2fv3(&((CT_Vec2f *)h->buffer)[h->num - 2],
                        &((CT_Vec2f *)h->buffer)[h->num - 1],
                        &points[i]) >= 0) {
      h->num--;
      len--;
    }
    ct_vector_push(h, &points[i]);
    CT_DEBUG("add h: %f,%f (%zu)", points[i].x, points[i].y, len);
    len++;
  }
  return len - 1;
}

size_t ct_convexhull2f(CT_Vec2f *points, size_t num, CT_Vector *hull) {
  if (num < 1) {
    return 0;
  }
  qsort(points, num, sizeof(CT_Vec2f), ct_compare2fv_xy);
  size_t len = hull2f(points, num, hull);
  ct_array_reverse8_imm(points, num);
  len += hull2f(points, num, hull);
  return len - (ct_compare2fv_xy((CT_Vec2f *)ct_vector_get(hull, 0),
                                 (CT_Vec2f *)ct_vector_get(hull, 1)) == 0);
}
