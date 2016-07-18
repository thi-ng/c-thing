#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "geom/clip/vatti.h"
#include "geom/isec.h"

// https://en.wikipedia.org/wiki/Vatti_clipping_algorithm
// based on: http://davis.wpi.edu/~matt/courses/clipping/

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

static int classify_node(CT_ClipNode *point, CT_ClipNode *p) {
  float i;
  int type      = 0;
  CT_Vec2f left = {-1e6, point->pos.y};
  for (CT_ClipNode *aux = p; aux->next; aux = aux->next) {
    type += ct_intersect_lines_simple(&left, &point->pos, &aux->pos,
                                      &aux->next->pos, &i);
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

static void insert_intersections(CT_ClipNode *s, CT_ClipNode *c) {
  float alpha_s, alpha_c;
  CT_Vec2f isec;
  for (CT_ClipNode *auxs = s; auxs->next; auxs = auxs->next) {
    if (!auxs->intersect) {
      for (CT_ClipNode *auxc = c; auxc->next; auxc = auxc->next) {
        if (!auxc->intersect) {
          if (ct_intersect_lines(&auxs->pos, &next_node(auxs->next)->pos,
                                 &auxc->pos, &next_node(auxc->next)->pos,
                                 &alpha_s, &alpha_c, &isec)) {
            CT_ClipNode *is = ct_create_clip_node(&isec, NULL, NULL, NULL, NULL,
                                                  1, 0, 0, alpha_s);
            CT_ClipNode *ic = ct_create_clip_node(&isec, NULL, NULL, NULL, NULL,
                                                  1, 0, 0, alpha_c);
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
            ct_create_clip_node(&crt->pos, old, NULL, NULL, NULL, 0, 0, 0, 0);
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

CT_ClipNode *ct_create_clip_node(CT_Vec2f *pos, CT_ClipNode *next,
                                 CT_ClipNode *prev, CT_ClipNode *nextPoly,
                                 CT_ClipNode *neighbor, int intersect,
                                 int entry, int visited, float alpha) {
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

CT_ClipNode *ct_create_polygon2f(CT_Vec2f *points, size_t num) {
  CT_ClipNode *poly = NULL;
  for (size_t i = 0; i < num; i++) {
    poly = ct_create_clip_node(&points[i], poly, NULL, NULL, NULL, 0, 0, 0, 0);
  }
  return poly;
}

CT_ClipNode *ct_clip_polygon2f(CT_ClipNode *s, CT_ClipNode *c, int mode) {
  CT_ClipNode *ends =
      ct_create_clip_node(&s->pos, NULL, last_node(s), NULL, NULL, 0, 0, 0, 0);
  CT_ClipNode *endc =
      ct_create_clip_node(&c->pos, NULL, last_node(c), NULL, NULL, 0, 0, 0, 0);
  insert_intersections(s, c);
  mark_nodes(s, classify_node(s, c), mode & 1);
  mark_nodes(c, classify_node(c, s), mode & 2);
  close_poly(s, ends);
  close_poly(c, endc);
  //ct_trace_polygon2f(s);
  //ct_trace_polygon2f(c);
  return build_result_poly(s);
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

void ct_free_polygon2f(CT_ClipNode *p) {
  CT_ClipNode *hold = p;
  do {
    CT_ClipNode *aux = p;
    p                = p->next;
    free(aux);
  } while (p && p != hold);
}
