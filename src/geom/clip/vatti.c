#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "geom/clip/vatti.h"
#include "geom/isec.h"

// https://en.wikipedia.org/wiki/Vatti_clipping_algorithm
// based on: http://davis.wpi.edu/~matt/courses/clipping/

static void insert_node(CT_ClipNode *ins,
                        CT_ClipNode *first,
                        CT_ClipNode *last) {
  CT_ClipNode *aux = first;
  while (aux != last && aux->alpha < ins->alpha) {
    aux = aux->next;
  }
  ins->next       = aux;
  ins->prev       = aux->prev;
  ins->prev->next = ins;
  ins->next->prev = ins;
}

static CT_ClipNode *next_node(CT_ClipNode *node) {
  while (node && node->intersect) {
    node = node->next;
  }
  return node;
}

static CT_ClipNode *last_node(CT_ClipNode *node) {
  if (node) {
    while (node->next) {
      node = node->next;
    }
  }
  return node;
}

static CT_ClipNode *first_node(CT_ClipNode *node) {
  CT_ClipNode *aux = node;
  if (aux) {
    do {
      aux = aux->next;
    } while (aux != node &&
             (!aux->intersect || (aux->intersect && aux->visited)));
  }
  return aux;
}

static void close_poly(CT_ClipContext *ctx,
                       CT_ClipNode *start,
                       CT_ClipNode *end) {
  end->prev->next = start;
  start->prev     = end->prev;
  ct_mpool_free_block(&ctx->pool, end);
}

static int classify_node(const CT_Vec2f *point, CT_ClipNode *poly) {
  float i;
  int type            = 0;
  const CT_Vec2f left = {-1e9, point->y};
  for (CT_ClipNode *aux = poly; aux->next; aux = aux->next) {
    type +=
        ct_intersect_lines_simple(&left, point, &aux->pos, &aux->next->pos, &i);
  }
  return type & 1;
}

static void mark_nodes(CT_ClipNode *p, int e, int invert) {
  if (invert) {
    e = !e;
  }
  while (p->next) {
    if (p->intersect) {
      p->entry = e;
      e        = !e;
    }
    p = p->next;
  }
}

static void insert_intersections(CT_ClipContext *ctx,
                                 CT_ClipNode *s,
                                 CT_ClipNode *c) {
  float alpha_s, alpha_c;
  CT_Vec2f isec;
  for (CT_ClipNode *auxs = s; auxs->next; auxs = auxs->next) {
    if (!auxs->intersect) {
      for (CT_ClipNode *auxc = c; auxc->next; auxc = auxc->next) {
        if (!auxc->intersect) {
          if (ct_intersect_lines(&auxs->pos, &next_node(auxs->next)->pos,
                                 &auxc->pos, &next_node(auxc->next)->pos,
                                 &alpha_s, &alpha_c, &isec)) {
            CT_ClipNode *is = ct_clip_create_node(ctx, &isec, NULL, NULL, NULL,
                                                  NULL, 1, 0, 0, alpha_s);
            CT_ClipNode *ic = ct_clip_create_node(ctx, &isec, NULL, NULL, NULL,
                                                  NULL, 1, 0, 0, alpha_c);
            is->neighbor = ic;
            ic->neighbor = is;
            insert_node(is, auxs, next_node(auxs->next));
            insert_node(ic, auxc, next_node(auxc->next));
          }
        }
      }
    }
  }
}

static CT_ClipNode *build_result_poly(CT_ClipContext *ctx, CT_ClipNode *start) {
  CT_ClipNode *crt, *result = NULL;
  while ((crt = first_node(start)) != start) {
    CT_ClipNode *old = NULL;
    do {
      int forward = crt->entry;
      while (1) {
        CT_ClipNode *new = ct_clip_create_node(ctx, &crt->pos, old, NULL, NULL,
                                               NULL, 0, 0, 0, 0);
        old          = new;
        crt->visited = 1;
        crt          = forward ? crt->next : crt->prev;
        if (crt->intersect) {
          crt->visited = 1;
          break;
        }
      }
      crt = crt->neighbor;
    } while (!crt->visited);
    old->nextPoly = result;
    result        = old;
  }
  return result;
}

ct_export int ct_clip_init_context(CT_ClipContext *ctx, size_t num) {
  return ct_mpool_init(&ctx->pool, num, sizeof(CT_ClipNode));
}

ct_export void ct_clip_free_context(CT_ClipContext *ctx) {
  ct_mpool_free(&ctx->pool);
}

ct_export void ct_clip_reset_context(CT_ClipContext *ctx) {
  ct_mpool_free_all_blocks(&ctx->pool);
}

ct_export CT_ClipNode *ct_clip_create_node(CT_ClipContext *ctx,
                                           const CT_Vec2f *pos,
                                           CT_ClipNode *next,
                                           CT_ClipNode *prev,
                                           CT_ClipNode *nextPoly,
                                           CT_ClipNode *neighbor,
                                           int intersect,
                                           int entry,
                                           int visited,
                                           float alpha) {
  CT_ClipNode *new = ct_mpool_alloc(&ctx->pool);
  new->pos         = *pos;
  new->next        = next;
  new->prev        = prev;
  if (prev)
    prev->next = new;
  if (next)
    next->prev   = new;
  new->nextPoly  = nextPoly;
  new->neighbor  = neighbor;
  new->intersect = intersect;
  new->entry     = entry;
  new->visited   = visited;
  new->alpha     = alpha;
  return new;
}

ct_export CT_ClipNode *ct_clip_create_polygon2f(CT_ClipContext *ctx,
                                                const CT_Vec2f *points,
                                                size_t num) {
  CT_ClipNode *poly = NULL;
  for (size_t i = 0; i < num; i++) {
    poly = ct_clip_create_node(ctx, &points[i], poly, NULL, NULL, NULL, 0, 0, 0,
                               0);
  }
  return poly;
}

ct_export CT_ClipNode *ct_clip_execute(CT_ClipContext *ctx,
                                       CT_ClipNode *s,
                                       CT_ClipNode *c,
                                       int mode) {
  CT_ClipNode *ends = ct_clip_create_node(ctx, &s->pos, NULL, last_node(s),
                                          NULL, NULL, 0, 0, 0, 0);
  CT_ClipNode *endc = ct_clip_create_node(ctx, &c->pos, NULL, last_node(c),
                                          NULL, NULL, 0, 0, 0, 0);
  insert_intersections(ctx, s, c);
  mark_nodes(s, classify_node(&s->pos, c), mode & 1);
  mark_nodes(c, classify_node(&c->pos, s), mode & 2);
  ct_clip_trace_polygon2f(s);
  ct_clip_trace_polygon2f(c);
  close_poly(ctx, s, ends);
  close_poly(ctx, c, endc);
  //ct_clip_trace_polygon2f(s);
  //ct_clip_trace_polygon2f(c);
  return build_result_poly(ctx, s);
}

ct_export void ct_clip_trace_node(const CT_ClipNode *p) {
  if (p)
    printf("%c%c%c [%3.3f, %3.3f] %f v:%10p n:%14p np:%14p\n",
           p->intersect ? 'I' : ' ', p->entry ? 'E' : ' ',
           p->visited ? 'X' : ' ', p->pos.x, p->pos.y, p->alpha, p, p->neighbor,
           p->nextPoly);
  else
    puts("NULL");
}

ct_export void ct_clip_trace_polygon2f(const CT_ClipNode *p) {
  CT_ClipNode *aux = (CT_ClipNode *)p;
  puts("");
  do {
    ct_clip_trace_node(aux);
    aux = aux->next;
  } while (aux && aux != p);
}

ct_export void ct_clip_free_node(CT_ClipContext *ctx, CT_ClipNode *p) {
  CT_ClipNode *hold = p;
  do {
    CT_ClipNode *aux = p;
    p                = p->next;
    ct_mpool_free_block(&ctx->pool, aux);
  } while (p && p != hold);
}
