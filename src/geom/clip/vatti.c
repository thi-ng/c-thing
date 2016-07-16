#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "geom/clip/vatti.h"

static void insert_node(CT_ClipNode *ins, CT_ClipNode *first,
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

static CT_ClipNode *create_node(CT_Vec2f *pos, CT_ClipNode *next,
                                CT_ClipNode *prev, CT_ClipNode *nextPoly,
                                CT_ClipNode *neighbor, int intersect, int entry,
                                int visited, float alpha) {
  CT_ClipNode *new     = malloc(sizeof(CT_ClipNode));
  new->pos             = *pos;
  new->next            = next;
  new->prev            = prev;
  if (prev) prev->next = new;
  if (next) next->prev = new;
  new->nextPoly        = nextPoly;
  new->neighbor        = neighbor;
  new->intersect       = intersect;
  new->entry           = entry;
  new->visited         = visited;
  new->alpha           = alpha;
  return new;
}

static CT_ClipNode *next_node(CT_ClipNode *n) {
  while (n && n->intersect) {
    n = n->next;
  }
  return n;
}

static CT_ClipNode *last_node(CT_ClipNode *n) {
  if (n) {
    while (n->next) {
      n = n->next;
    }
  }
  return n;
}

static CT_ClipNode *first_node(CT_ClipNode *p) {
  CT_ClipNode *aux = p;
  if (aux) {
    do {
      aux = aux->next;
    } while (aux != p && (!aux->intersect || (aux->intersect && aux->visited)));
  }
  return aux;
}

static void close_poly(CT_ClipNode *start, CT_ClipNode *end) {
  end->prev->next = start;
  start->prev     = end->prev;
  free(end);
}

static int intersect_simple(CT_ClipNode *p1, CT_ClipNode *p2, CT_ClipNode *q1,
                            CT_ClipNode *q2, float *tp) {
  CT_Vec2f *a = &p1->pos;
  CT_Vec2f *b = &p2->pos;
  CT_Vec2f *c = &q1->pos;
  CT_Vec2f *d = &q2->pos;
  float dabx  = b->x - a->x;
  float daby  = b->y - a->y;
  float dcdx  = d->x - c->x;
  float dcdy  = d->y - c->y;
  float det   = dabx * dcdy - daby * dcdx;
  if (fabs(det) < 1e-4) return 0;
  float dacx = c->x - a->x;
  float dacy = c->y - a->y;
  *tp        = (dacx * dcdy - dacy * dcdx) / det;
  float tq   = (daby * dacx - dabx * dacy) / det;
  return !(*tp < 0 || *tp > 1.0 || tq < 0 || tq > 1.0);
}

static int intersect(CT_ClipNode *p1, CT_ClipNode *p2, CT_ClipNode *q1,
                     CT_ClipNode *q2, float *alpha_p, float *alpha_q,
                     CT_Vec2f *isec) {
  float tp;
  if (intersect_simple(p1, p2, q1, q2, &tp)) {
    const CT_Vec2f *a = &p1->pos;
    const CT_Vec2f *b = &p2->pos;
    const CT_Vec2f *c = &q1->pos;
    ct_mix2fv(a, b, tp, isec);
    *alpha_p = ct_dist2fv(a, isec) / ct_dist2fv(a, b);
    *alpha_q = ct_dist2fv(c, isec) / ct_dist2fv(c, &q2->pos);
    return 1;
  }
  return 0;
}

static int classify_node(CT_ClipNode *point, CT_ClipNode *p) {
  float i;
  int type         = 0;
  CT_Vec2f pos     = {0, point->pos.y};
  CT_ClipNode left = {.pos = {-FLT_MAX, point->pos.y}};
  for (CT_ClipNode *aux = p; aux->next; aux = aux->next) {
    if (intersect_simple(&left, point, aux, aux->next, &i)) {
      type++;
    }
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

static void insert_intersections(CT_ClipNode *s, CT_ClipNode *c,
                                 CT_ClipNode *auxs, CT_ClipNode *auxc) {
  float alpha_s, alpha_c;
  CT_Vec2f isec;
  for (auxs = s; auxs->next; auxs = auxs->next) {
    if (!auxs->intersect) {
      for (auxc = c; auxc->next; auxc = auxc->next) {
        if (!auxc->intersect) {
          if (intersect(auxs, next_node(auxs->next), auxc,
                        next_node(auxc->next), &alpha_s, &alpha_c, &isec)) {
            CT_ClipNode *is =
                create_node(&isec, NULL, NULL, NULL, NULL, 1, 0, 0, alpha_s);
            CT_ClipNode *ic =
                create_node(&isec, NULL, NULL, NULL, NULL, 1, 0, 0, alpha_c);
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

static CT_ClipNode *build_result_poly(CT_ClipNode *start) {
  CT_ClipNode *crt, *result = NULL;
  while ((crt = first_node(start)) != start) {
    CT_ClipNode *old = NULL;
    do {
      int forward = crt->entry;
      while (1) {
        CT_ClipNode *new =
            create_node(&crt->pos, old, NULL, NULL, NULL, 0, 0, 0, 0);
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

CT_ClipNode *ct_clip_polygon2f(CT_ClipNode *s, CT_ClipNode *c, int mode) {
  CT_ClipNode *auxs = last_node(s);
  CT_ClipNode *ends = create_node(&s->pos, NULL, auxs, NULL, NULL, 0, 0, 0, 0);
  CT_ClipNode *auxc = last_node(c);
  CT_ClipNode *endc = create_node(&c->pos, NULL, auxc, NULL, NULL, 0, 0, 0, 0);
  insert_intersections(s, c, auxs, auxc);
  mark_nodes(s, classify_node(s, c), mode & 1);
  mark_nodes(c, classify_node(c, s), mode & 2);
  close_poly(s, ends);
  close_poly(c, endc);
  return build_result_poly(s);
}

CT_ClipNode *ct_create_polygon2f(CT_Vec2f *points, size_t num) {
  CT_ClipNode *poly = NULL;
  for (size_t i = 0; i < num; i++) {
    poly = create_node(&points[i], poly, NULL, NULL, NULL, 0, 0, 0, 0);
  }
  return poly;
}

void ct_trace_clip_node(CT_ClipNode *p) {
  if (p)
    printf("%c%c%c [%3.3f, %3.3f] %f v:%10p n:%14p np:%14p\n",
           p->intersect ? 'I' : ' ', p->entry ? 'E' : ' ',
           p->visited ? 'X' : ' ', p->pos.x, p->pos.y, p->alpha, p, p->neighbor,
           p->nextPoly);
  else
    puts("NULL");
}

void ct_trace_polygon2f(CT_ClipNode *p) {
  CT_ClipNode *aux = p;
  puts("");
  do {
    ct_trace_clip_node(aux);
    aux = aux->next;
  } while (aux && aux != p);
}

void ct_free_poly(CT_ClipNode *p) {
  CT_ClipNode *hold = p;
  do {
    CT_ClipNode *aux = p;
    p                = p->next;
    free(aux);
  } while (p && p != hold);
}
