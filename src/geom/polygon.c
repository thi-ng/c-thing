#include "geom/polygon.h"
#include "geom/isec.h"

// http://www.inf.usi.ch/hormann/papers/Greiner.1998.ECO.pdf
// http://davis.wpi.edu/~matt/courses/clipping/
// http://syntheti.cc/article/polygon-clipping/
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
  CT_Cons *result = NULL, *rtail = NULL;
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
    rtail  = ct_cons_append(clipped, rtail, &ctx->cpool);
    result = result ? result : rtail;
  }
  return result;
}

// public functions

int ct_polygon2f_init(CT_Polygon2f *poly, CT_Vec2f *points, size_t num) {
  poly->first     = NULL;
  poly->firstIsec = NULL;
  poly->lastProc  = NULL;
  poly->num       = 0;
  if (!ct_mpool_init(&poly->pool, CT_MAX(num, 16), sizeof(CT_PolyVertex))) {
    if (points) {
      for (size_t i = 0; i < num; i++) {
        ct_polygon2f_add_vertex(poly, &points[i]);
      }
    }
    return 0;
  }
  return 1;
}

int ct_polygon2f_init_cons(CT_Polygon2f *poly, CT_Cons *points, size_t num) {
  if (ct_polygon2f_init(poly, NULL, num)) {
    return 1;
  }
  while (points) {
    ct_polygon2f_add_vertex(poly, (CT_Vec2f *)points->value);
    points = points->next;
  }
  return 0;
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

CT_Vec2f *ct_polygon2f_vertices(CT_Polygon2f *poly, CT_Vec2f *vertices) {
  CT_PolyVertex *v = poly->first;
  CT_Vec2f *ptr    = vertices;
  do {
    *ptr++ = v->pos;
    v      = v->next;
  } while (v != poly->first);
  return vertices;
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
  ct_mpool_free(&ctx->cpool);
  ct_mpool_free(&ctx->ppool);
}

void ct_polygon2f_clip_ctx_reset(CT_PolyClipContext *ctx) {
  CT_Cons *head = ctx->result;
  while (head) {
    ct_polygon2f_free(head->value);
    head = head->next;
  }
  ctx->result = NULL;
  ct_mpool_reset(&ctx->cpool);
  ct_mpool_reset(&ctx->ppool);
}

/// polygon offsetting

static void trace_edge(const CT_PolyEdge *e) {
  CT_DEBUG("e: %f,%f -> %f,%f n: %f,%f", e->curr.x, e->curr.y, e->next.x,
           e->next.y, e->normal.x, e->normal.y);
}

static void set_edge_normal(CT_PolyEdge *e) {
  ct_normalize2f_imm(
      ct_perpendicular2f_imm(ct_sub2fv(&e->next, &e->curr, &e->normal)), 1);
}

static CT_PolyEdge *init_edge(CT_PolyEdge *e,
                              const CT_Vec2f *a,
                              const CT_Vec2f *b) {
  e->curr = *a;
  e->next = *b;
  set_edge_normal(e);
  trace_edge(e);
  return e;
}

static CT_PolyEdge *init_edge_offset(CT_PolyEdge *dest,
                                     const CT_PolyEdge *src,
                                     float dist) {
  CT_Vec2f n;
  ct_mul2fn(&src->normal, dist, &n);
  ct_add2fv(&src->curr, &n, &dest->curr);
  ct_add2fv(&src->next, &n, &dest->next);
  set_edge_normal(dest);
  trace_edge(dest);
  return dest;
}

static void free_vertex(void *v, void *p) {
  ct_mpool_free_block((CT_MPool *)p, v);
}

static CT_Cons *create_arc(CT_PolyOffsetContext *ctx,
                           CT_Cons *vertices,
                           const CT_Vec2f *c,
                           float radius,
                           const CT_Vec2f *a,
                           const CT_Vec2f *b,
                           int outwards) {
  float ta = atan2f(a->y - c->y, a->x - c->x);
  float tb = atan2f(b->y - c->y, b->x - c->x);
  if (ta < 0) {
    ta += CT_TAU;
  }
  if (tb < 0) {
    tb += CT_TAU;
  }
  float theta = (ta > tb) ? (ta - tb) : (ta + CT_TAU - tb);
  float ts    = (outwards ? -theta : CT_TAU - theta) / ctx->res;
  for (size_t i = 1; i < ctx->res; i++) {
    CT_Vec2f *v = ct_mpool_alloc(&ctx->vpool);
    ct_add2fv_imm(ct_cartesian2f_imm(ct_set2fxy(v, radius, ta + ts * i)), c);
    CT_DEBUG("add v: %f,%f", v->x, v->y);
    vertices = ct_cons_append(v, vertices, &ctx->cpool);
  }
  return vertices;
}

static CT_Cons *compute_offset_vertices(CT_PolyOffsetContext *ctx,
                                        float dist,
                                        size_t *vertCount) {
  CT_PolyEdge *edges = malloc(ctx->num * sizeof(CT_PolyEdge));
  size_t numE = ctx->num, numV = 0, i;
  for (i = 0; i < numE; i++) {
    init_edge_offset(&edges[i], &ctx->edges[i], dist);
  }
  const int outward    = dist > 0;
  const int meterLimit = 1 - (ctx->res < 0);
  CT_INFO("res: %d, ml: %d", ctx->res, meterLimit);
  dist              = fabs(dist);
  CT_Cons *vertices = NULL, *vcurr = NULL;
  for (i = 0; i < numE; i++) {
    CT_PolyEdge *a = &edges[i];
    CT_PolyEdge *b = &edges[(i + numE - 1) % numE];
    CT_Vec2f isec;
    float _a, _b;
    if (meterLimit <= ct_intersect_lines(&b->curr, &b->next, &a->curr, &a->next,
                                         &isec, &_a, &_b)) {
      CT_Vec2f *v = ct_mpool_alloc(&ctx->vpool);
      *v          = isec;
      CT_DEBUG("add v: %f,%f", v->x, v->y);
      vcurr    = ct_cons_append(v, vcurr, &ctx->cpool);
      vertices = vertices ? vertices : vcurr;
      numV++;
    } else {
      CT_Vec2f *v = ct_mpool_alloc(&ctx->vpool);
      *v          = b->next;
      CT_DEBUG("add v: %f,%f", v->x, v->y);
      vcurr    = ct_cons_append(v, vcurr, &ctx->cpool);
      vertices = vertices ? vertices : vcurr;
      vcurr    = create_arc(ctx, vcurr, &ctx->edges[i].curr, dist, &b->next,
                         &a->curr, outward);
      v  = ct_mpool_alloc(&ctx->vpool);
      *v = a->curr;
      CT_DEBUG("add v: %f,%f", v->x, v->y);
      vcurr = ct_cons_append(v, vcurr, &ctx->cpool);
      numV += ctx->res;
    }
  }
  free(edges);

  *vertCount = numV;
  return vertices;
}

static CT_Polygon2f *compute_offset_poly(CT_PolyOffsetContext *ctx,
                                         float dist) {
  size_t numV       = 0;
  CT_Cons *vertices = compute_offset_vertices(ctx, dist, &numV);
  CT_Polygon2f *s   = ct_mpool_alloc(&ctx->clip.ppool);
  CT_Polygon2f *c   = ct_mpool_alloc(&ctx->clip.ppool);
  ct_polygon2f_init_cons(s, vertices, numV);
  ct_polygon2f_init_cons(c, vertices, numV);
  ct_cons_free_all(vertices, &ctx->cpool, free_vertex, &ctx->vpool);
  CT_Cons *rc = ct_polygon2f_clip(&ctx->clip, s, c, CT_CLIP_UNION);
  CT_Polygon2f *result;
  if (rc) {
    result = rc->value;
    if (rc->next) {
      CT_Cons *r = rc->next, *rn;
      while (r) {
        rn = r->next;
        ct_polygon2f_free(r->value);
        ct_mpool_free_block(&ctx->clip.ppool, r->value);
        ct_mpool_free_block(&ctx->clip.cpool, r);
        r = rn;
      }
      rc->next = NULL;
      ct_polygon2f_free(s);
      ct_mpool_free_block(&ctx->clip.ppool, s);
    }
  } else {
    ctx->clip.result = ct_cons(s, NULL, &ctx->clip.cpool);
    result           = s;
  }
  ct_polygon2f_free(c);
  ct_mpool_free_block(&ctx->clip.ppool, c);
  return result;
}

int ct_polygon2f_offset_ctx_init(CT_PolyOffsetContext *ctx, size_t poolSize) {
  return ct_mpool_init(&ctx->vpool, poolSize * 4, sizeof(CT_Vec2f)) ||
         ct_mpool_init(&ctx->cpool, poolSize, sizeof(CT_Cons)) ||
         ct_polygon2f_clip_ctx_init(&ctx->clip, 4);
}

CT_Polygon2f *ct_polygon2f_offset_points(CT_PolyOffsetContext *ctx,
                                         const CT_Vec2f *points,
                                         size_t num,
                                         float dist,
                                         int8_t res) {
  CT_Polygon2f *result = NULL;
  ctx->edges           = malloc(num * sizeof(CT_PolyEdge));
  CT_CHECK_MEM(ctx->edges);
  for (size_t i = 0; i < num; i++) {
    init_edge(&ctx->edges[i], &points[i], &points[(i + 1) % num]);
  }
  ctx->num = num;
  ctx->res = res >= 0 ? res : -1;
  result   = compute_offset_poly(ctx, dist);
  free(ctx->edges);
  ctx->edges = NULL;
fail:
  return result;
}

void ct_polygon2f_offset_ctx_free(CT_PolyOffsetContext *ctx) {
  ct_polygon2f_clip_ctx_free(&ctx->clip);
  ct_mpool_free(&ctx->cpool);
  ct_mpool_free(&ctx->vpool);
}

void ct_polygon2f_offset_ctx_reset(CT_PolyOffsetContext *ctx) {
  ct_polygon2f_clip_ctx_reset(&ctx->clip);
  ct_mpool_reset(&ctx->cpool);
  ct_mpool_reset(&ctx->vpool);
}
