#include <math.h>

#include "geom/circle.h"
#include "geom/isec.h"
#include "math/poisson.h"

static int find_candidate(CT_QTNode *node, void *s) {
  CT_Circle2f *disc = (CT_Circle2f *)s;
  CT_Vec2f p        = {node->x + node->w, node->y + node->h};
  if (ct_intersect_rect_circle(&node->pos, &p, &disc->pos, disc->r)) {
    if (node->type == CT_TREE_LEAF) {
      float d = ct_dist2fv(&disc->pos, node->point);
      if (d < disc->r) {
        disc->r = d;
      }
    }
    return 0;
  }
  return 1;
}

CT_EXPORT CT_Vec2f *ct_poisson_sample2f(CT_Quadtree *t, float radius,
                                        size_t num, CT_Vec2f *out) {
  float maxD = 0;
  float w    = t->root.w;
  float h    = t->root.h;
  CT_Circle2f disc;
  for (size_t i = 0; i < num; i++) {
    ct_set2fxy(&disc.pos, ct_rand_normpos() * w, ct_rand_normpos() * h);
    disc.r = radius;
    ct_qtree_visit(t, find_candidate, &disc);
    if (disc.r > maxD) {
      maxD = disc.r;
      *out = disc.pos;
    }
  }
  if (maxD >= radius) {
    //CT_INFO("final dist: %f (%f, %f)", maxD, out->x, out->y);
    ct_qtree_insert(t, out, NULL);
    return out;
  }
  return NULL;
}
