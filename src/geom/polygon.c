#include "geom/polygon.h"
#include "geom/isec.h"

// http://www.inf.usi.ch/hormann/papers/Greiner.1998.ECO.pdf
// http://davis.wpi.edu/~matt/courses/clipping/
// https://github.com/tcql/greiner-hormann/
// https://github.com/w8r/GreinerHormann/

static CT_PolyVertex *init_vertex(CT_PolyVertex *v, const CT_Vec2f *pos) {
  v->pos     = *pos;
  v->next    = NULL;
  v->prev    = NULL;
  v->pair    = NULL;
  v->isec    = 0;
  v->entry   = 1;
  v->visited = 0;
  v->dist    = 0;
  return v;
}

static CT_PolyVertex *init_isec_vertex(CT_PolyVertex *v,
                                       CT_Vec2f *pos,
                                       float dist) {
  v->pos     = *pos;
  v->next    = NULL;
  v->prev    = NULL;
  v->dist    = dist;
  v->isec    = 1;
  v->entry   = 0;
  v->visited = 0;
  return v;
}

static void insert_vertex(CT_Polygon2f *poly,
                          CT_PolyVertex *v,
                          CT_PolyVertex *start,
                          CT_PolyVertex *end) {
  CT_PolyVertex *curr = start, *prev;
  while (curr != end && curr->dist < v->dist) {
    curr = curr->next;
  }
  v->next    = curr;
  prev       = curr->prev;
  v->prev    = prev;
  prev->next = v;
  curr->prev = v;
  poly->num++;
}

static void visit_vertex(CT_PolyVertex *v) {
  v->visited = 1;
  if (v->pair && !v->pair->visited) {
    v->pair->visited = 1;
  }
}

static CT_PolyVertex *next_non_isec(CT_PolyVertex *v) {
  while (v->isec) {
    v = v->next;
  }
  return v;
}

static CT_PolyVertex *first_isec(CT_Polygon2f *poly) {
  CT_PolyVertex *v = poly->firstIsec ? poly->firstIsec : poly->first;
  do {
    if (v->isec && !v->visited) {
      break;
    }
    v = v->next;
  } while (v != poly->first);
  poly->firstIsec = v;
  return v;
}

static int poly_has_unprocessed(CT_Polygon2f *poly) {
  CT_PolyVertex *v = poly->lastProc ? poly->lastProc : poly->first;
  do {
    if (v->isec && !v->visited) {
      poly->lastProc = v;
      return 1;
    }
    v = v->next;
  } while (v != poly->first);
  poly->lastProc = NULL;
  return 0;
}

static void insert_isec_vertices(CT_Polygon2f *src, CT_Polygon2f *clip) {
  CT_PolyVertex *s = src->first;
  CT_PolyVertex *c = clip->first;
  do {
    if (!s->isec) {
      do {
        if (!c->isec) {
          float alpha, beta;
          CT_Vec2f isec;
          CT_PolyVertex *ns = next_non_isec(s->next);
          CT_PolyVertex *nc = next_non_isec(c->next);
          if (1 == ct_intersect_lines(&s->pos, &ns->pos, &c->pos, &nc->pos,
                                      &isec, &alpha, &beta)) {
            CT_PolyVertex *is =
                init_isec_vertex(ct_mpool_alloc(&src->pool), &isec, alpha);
            CT_PolyVertex *ic =
                init_isec_vertex(ct_mpool_alloc(&clip->pool), &isec, beta);
            is->pair = ic;
            ic->pair = is;
            insert_vertex(src, is, s, ns);
            insert_vertex(clip, ic, c, nc);
          }
        }
        c = c->next;
      } while (c != clip->first);
    }
    s = s->next;
  } while (s != src->first);
}

static void mark_entry_vertices(CT_Polygon2f *poly, int state) {
  CT_PolyVertex *v = poly->first;
  do {
    if (v->isec) {
      v->entry = state;
      state    = !state;
    }
    v = v->next;
  } while (v != poly->first);
}

static CT_Cons *build_clip_polys(CT_PolyClipContext *ctx, CT_Polygon2f *src) {
  CT_Cons *result = NULL;
  while (poly_has_unprocessed(src)) {
    CT_PolyVertex *curr   = first_isec(src);
    CT_Polygon2f *clipped = ct_mpool_alloc(&ctx->ppool);
    ct_polygon2f_init(clipped, &curr->pos, 1);
    do {
      visit_vertex(curr);
      if (curr->entry) {
        do {
          curr = curr->next;
          ct_polygon2f_add_vertex(clipped, &curr->pos);
        } while (!curr->isec);
      } else {
        do {
          curr = curr->prev;
          ct_polygon2f_add_vertex(clipped, &curr->pos);
        } while (!curr->isec);
      }
      curr = curr->pair;
    } while (!curr->visited);
    CT_DEBUG("add poly: %p (%zu points)", clipped, clipped->num);
    result = ct_cons(clipped, result, &ctx->cpool);
  }
  return result;
}

// public functions

int ct_polygon2f_init(CT_Polygon2f *poly, CT_Vec2f *points, size_t num) {
  poly->first     = NULL;
  poly->firstIsec = NULL;
  poly->lastProc  = NULL;
  poly->num       = 0;
  if (!ct_mpool_init(&poly->pool, MAX(num, 16), sizeof(CT_PolyVertex))) {
    if (points) {
      for (size_t i = 0; i < num; i++) {
        ct_polygon2f_add_vertex(poly, &points[i]);
      }
    }
    return 0;
  }
  return 1;
}

void ct_polygon2f_free(CT_Polygon2f *poly) {
  ct_mpool_free(&poly->pool);
}

CT_Polygon2f *ct_polygon2f_add_vertex(CT_Polygon2f *poly, const CT_Vec2f *pos) {
  CT_PolyVertex *v = init_vertex(ct_mpool_alloc(&poly->pool), pos);
  if (poly->first) {
    CT_PolyVertex *next = poly->first;
    CT_PolyVertex *prev = next->prev;
    next->prev          = v;
    v->next             = next;
    v->prev             = prev;
    prev->next          = v;
  } else {
    poly->first = v;
    v->next     = v;
    v->prev     = v;
  }
  poly->num++;
  return poly;
}

int ct_polygon2f_point_inside(const CT_Polygon2f *poly, const CT_Vec2f *v) {
  int flag              = 0;
  CT_PolyVertex *vertex = (CT_PolyVertex *)poly->first;
  CT_PolyVertex *next   = vertex->next;
  float x               = v->x;
  float y               = v->y;
  do {
    CT_Vec2f *vp = &vertex->pos;
    CT_Vec2f *np = &next->pos;
    if (((vp->y < y && np->y >= y) || (np->y < y && vp->y >= y)) &&
        (vp->x <= x || np->x <= x)) {
      flag ^= ((vp->x + (y - vp->y) / (np->y - vp->y) * (np->x - vp->x)) < x);
    }
    vertex = next;
    next   = vertex->next ? vertex->next : poly->first;
  } while (vertex != poly->first);
  return flag;
}

CT_Cons *ct_polygon2f_clip(CT_PolyClipContext *ctx,
                           CT_Polygon2f *src,
                           CT_Polygon2f *clip,
                           size_t mode) {
  insert_isec_vertices(src, clip);

  int srcInClip = ct_polygon2f_point_inside(clip, &src->first->pos);
  int clipInSrc = ct_polygon2f_point_inside(src, &clip->first->pos);

  mark_entry_vertices(src, (mode & 1) ^ srcInClip);
  mark_entry_vertices(clip, ((mode >> 1) & 1) ^ clipInSrc);

  CT_Cons *result = build_clip_polys(ctx, src);

  if (!result) {
    if (srcInClip) {
      result = ct_cons(src, result, &ctx->cpool);
    }
    if (clipInSrc) {
      result = ct_cons(clip, result, &ctx->cpool);
    }
  }
  return ctx->result = result;
}

int ct_polygon2f_clip_ctx_init(CT_PolyClipContext *ctx, size_t num) {
  ctx->result = NULL;
  return (ct_mpool_init(&ctx->ppool, num, sizeof(CT_Polygon2f)) ||
          ct_mpool_init(&ctx->cpool, num, sizeof(CT_Cons)));
}

void ct_polygon2f_clip_ctx_free(CT_PolyClipContext *ctx) {
  CT_Cons *head = ctx->result;
  while (head) {
    ct_polygon2f_free(head->value);
    head = head->next;
  }
  ct_mpool_free(&ctx->ppool);
  ct_mpool_free(&ctx->cpool);
}
