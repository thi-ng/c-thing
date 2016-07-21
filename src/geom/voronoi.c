#include "geom/voronoi.h"

ct_inline void dec_ref(CT_MPool *pool, CT_VOVertex *v) {
  v->refs--;
  if (!v->refs) {
    ct_mpool_free_block(pool, v);
  }
}

ct_inline void inc_ref(CT_VOVertex *v) {
  v->refs++;
}

ct_inline CT_VOVertex *set_vertex_id(CT_Voronoi *vor, CT_VOVertex *v) {
  v->id = vor->numVertices;
  vor->numVertices++;
  return v;
}

ct_inline CT_VOVertex *leftreg(CT_VOHalfEdge *he, CT_VOVertex *fail) {
  if (!he->parent) {
    return fail;
  }
  return (he->side == 0 ? he->parent->reg[0] : he->parent->reg[1]);
}

ct_inline CT_VOVertex *rightreg(CT_VOHalfEdge *he, CT_VOVertex *fail) {
  if (!he->parent) {
    return fail;
  }
  return (he->side == 0 ? he->parent->reg[1] : he->parent->reg[0]);
}

ct_inline int heap_is_empty(CT_VOHeap *heap) {
  return heap->count == 0;
}

static int heap_bucket(CT_VOHeap *heap, CT_VOHalfEdge *he) {
  int bucket = (he->key - heap->miny) / heap->deltay * heap->hashSize;
  if (bucket < 0) {
    bucket = 0;
  } else if (bucket >= heap->hashSize) {
    bucket = heap->hashSize - 1;
  } else if (bucket < heap->min) {
    heap->min = bucket;
  }
  return bucket;
}

static void heap_insert(CT_VOHeap *heap,
                        CT_VOHalfEdge *he,
                        CT_VOVertex *v,
                        float offset) {
  CT_VOHalfEdge *last, *next;
  inc_ref(v);
  he->vertex = v;
  he->key    = v->pos.y + offset;
  last       = &heap->hash[heap_bucket(heap, he)];
  while ((next = last->next) &&
         (he->key > next->key ||
          (he->key == next->key && v->pos.x > next->vertex->pos.x))) {
    last = next;
  }
  he->next   = last->next;
  last->next = he;
  heap->count++;
}

static void heap_delete(CT_VOHeap *heap, CT_VOHalfEdge *he, CT_MPool *vpool) {
  if (he->vertex) {
    CT_VOHalfEdge *last = &heap->hash[heap_bucket(heap, he)];
    while (last->next != he) {
      last = last->next;
    }
    last->next = he->next;
    heap->count--;
    dec_ref(vpool, he->vertex);
    he->vertex = NULL;
  }
}

static CT_Vec2f heap_get_min(CT_VOHeap *heap) {
  while (!heap->hash[heap->min].next) {
    heap->min++;
  }
  CT_VOHalfEdge *he = heap->hash[heap->min].next;
  CT_Vec2f v        = {he->vertex->pos.x, he->key};
  return v;
}

static CT_VOHalfEdge *heap_extract_min(CT_VOHeap *heap) {
  CT_VOHalfEdge *curr = &heap->hash[heap->min];
  CT_VOHalfEdge *next = curr->next;
  curr->next          = next->next;
  heap->count--;
  return next;
}

static CT_VOEdge *bisect(CT_Voronoi *vor, CT_VOVertex *v1, CT_VOVertex *v2) {
  CT_VOEdge *newedge = ct_mpool_alloc(&vor->epool);
  newedge->reg[0]    = v1;
  newedge->reg[1]    = v2;
  inc_ref(v1);
  inc_ref(v2);
  newedge->ep[0] = NULL;
  newedge->ep[1] = NULL;
  float dx       = v2->pos.x - v1->pos.x;
  float dy       = v2->pos.y - v1->pos.y;
  float adx      = dx > 0 ? dx : -dx;
  float ady      = dy > 0 ? dy : -dy;
  newedge->c     = v1->pos.x * dx + v1->pos.y * dy + (dx * dx + dy * dy) * 0.5;
  if (adx > ady) {
    newedge->a = 1.0;
    newedge->b = dy / dx;
    newedge->c /= dx;
  } else {
    newedge->b = 1.0;
    newedge->a = dx / dy;
    newedge->c /= dy;
  }
  newedge->id = vor->numEdges;
  vor->numEdges++;
  return newedge;
}

static CT_VOVertex *intersect(CT_Voronoi *vor,
                              CT_VOHalfEdge *ea,
                              CT_VOHalfEdge *eb) {
  CT_VOEdge *e1 = ea->parent;
  CT_VOEdge *e2 = eb->parent;
  if (!e1 || !e2) {
    return NULL;
  }
  if (e1->reg[1] == e2->reg[1]) {
    return NULL;
  }

  float d = e1->a * e2->b - e1->b * e2->a;
  if (-1.0e-10 < d && d < 1.0e-10) {
    return NULL;
  }

  d          = 1.0 / d;
  float xint = (e1->c * e2->b - e2->c * e1->b) * d;
  float yint = (e2->c * e1->a - e1->c * e2->a) * d;

  CT_VOEdge *e;
  CT_VOHalfEdge *he;
  if ((e1->reg[1]->pos.y < e2->reg[1]->pos.y) ||
      (e1->reg[1]->pos.y == e2->reg[1]->pos.y &&
       e1->reg[1]->pos.x < e2->reg[1]->pos.x)) {
    he = ea;
    e  = e1;
  } else {
    he = eb;
    e  = e2;
  }
  int is_right = (xint >= e->reg[1]->pos.x);
  if ((is_right && he->side == 0) || (!is_right && he->side == 1)) {
    return NULL;
  }
  CT_VOVertex *v = ct_mpool_alloc(&vor->vpool);
  v->refs        = 0;
  v->pos.x       = xint;
  v->pos.y       = yint;
  return v;
}

static int is_right_of(CT_VOHalfEdge *el, CT_Vec2f *p) {
  int above;
  CT_VOEdge *e         = el->parent;
  CT_VOVertex *topsite = e->reg[1];
  int is_right         = (p->x > topsite->pos.x);
  if (is_right && el->side == 0) {
    return 1;
  }
  if (!is_right && el->side == 1) {
    return 0;
  }

  if (e->a == 1.0) {
    float dxp = p->x - topsite->pos.x;
    float dyp = p->y - topsite->pos.y;
    int fast  = 0;
    if ((!is_right && e->b < 0.0) || (is_right && e->b >= 0.0)) {
      above = (dyp >= e->b * dxp);
      fast  = above;
    } else {
      above = p->x + p->y * e->b > e->c;
      if (e->b < 0.0) {
        above = !above;
      }
      if (!above) {
        fast = 1;
      }
    }
    if (!fast) {
      float dxs = topsite->pos.x - (e->reg[0])->pos.x;
      above     = (e->b * (dxp * dxp - dyp * dyp) <
               dxs * dyp * (1.0 + 2.0 * dxp / dxs + e->b * e->b));
      if (e->b < 0.0) {
        above = !above;
      }
    }
  } else {  // e->b==1.0
    float yl = e->c - e->a * p->x;
    float t1 = p->y - yl;
    float t2 = p->x - topsite->pos.x;
    float t3 = yl - topsite->pos.y;
    above    = (t1 * t1 > t2 * t2 + t3 * t3);
  }
  return (el->side == 0 ? above : !above);
}

static void edge_set_endpoint(CT_Voronoi *vor,
                              CT_VOEdge *e,
                              int side,
                              CT_VOVertex *s,
                              CT_VOEdgeHandler handler,
                              void *state) {
  e->ep[side] = s;
  inc_ref(s);
  if (e->ep[1 - side]) {
    handler(e, state);
    dec_ref(&vor->vpool, e->reg[0]);
    dec_ref(&vor->vpool, e->reg[1]);
    ct_mpool_free_block(&vor->epool, e);
  }
}

static void edgelist_insert(CT_VOHalfEdge *lb, CT_VOHalfEdge *new) {
  new->left       = lb;
  new->right      = lb->right;
  lb->right->left = new;
  lb->right       = new;
}

static CT_VOHalfEdge *edgelist_get_hash(CT_VOEdgeList *el, int b) {
  CT_VOHalfEdge *he;
  if (b < 0 || b >= el->hashSize) {
    return NULL;
  }
  he = el->hash[b];
  if (he == NULL || he->parent != (CT_VOEdge *)VORONOI_DELETED) {
    return he;
  }

  // he is a deleted edge
  el->hash[b] = NULL;
  he->refs--;
  if (he->refs == 0) {
    ct_mpool_free_block(&el->pool, he);
  }
  return NULL;
}

static CT_VOHalfEdge *edgelist_leftbnd(CT_VOEdgeList *el, CT_Vec2f *p) {
  // get close to desired halfedge
  int bucket = (p->x - el->minx) / el->deltax * el->hashSize;
  if (bucket < 0) {
    bucket = 0;
  } else if (bucket >= el->hashSize) {
    bucket = el->hashSize - 1;
  }
  CT_VOHalfEdge *he = edgelist_get_hash(el, bucket);
  if (!he) {
    for (size_t i = 1; 1; i++) {
      if ((he = edgelist_get_hash(el, bucket - i))) {
        break;
      }
      if ((he = edgelist_get_hash(el, bucket + i))) {
        break;
      }
    }
  }
  // scan list of halfedges for correct one
  if (he == el->leftend || (he != el->rightend && is_right_of(he, p))) {
    do {
      he = he->right;
    } while (he != el->rightend && is_right_of(he, p));
    he = he->left;
  } else {
    do {
      he = he->left;
    } while (he != el->leftend && !is_right_of(he, p));
  }

  if (bucket > 0 && bucket < el->hashSize - 1) {
    if (el->hash[bucket]) {
      el->hash[bucket]->refs--;
    }
    el->hash[bucket] = he;
    el->hash[bucket]->refs++;
  }
  return he;
}

static CT_VOHalfEdge *make_halfedge(CT_VOEdgeList *el, CT_VOEdge *e, int side) {
  CT_VOHalfEdge *he = ct_mpool_alloc(&el->pool);
  he->parent        = e;
  he->side          = side;
  he->next          = NULL;
  he->vertex        = NULL;
  he->refs          = 0;
  return he;
}

static void halfedge_delete(CT_VOHalfEdge *he) {
  he->left->right = he->right;
  he->right->left = he->left;
  he->parent      = (CT_VOEdge *)VORONOI_DELETED;
}

static int heap_init(CT_VOHeap *heap, size_t num) {
  heap->count    = 0;
  heap->min      = 0;
  heap->hashSize = 4 * (uint32_t)sqrt(num);
  heap->hash     = calloc(heap->hashSize, sizeof(CT_VOHalfEdge));
  return 0;
}

static int edgelist_init(CT_VOEdgeList *el, size_t num) {
  el->hashSize = 2 * (uint32_t)sqrt(num);
  el->hash     = calloc(el->hashSize, sizeof(CT_VOHalfEdge *));
  if (!ct_mpool_init(&el->pool, num, sizeof(CT_VOHalfEdge))) {
    el->leftend                = make_halfedge(el, NULL, 0);
    el->rightend               = make_halfedge(el, NULL, 0);
    el->leftend->left          = NULL;
    el->leftend->right         = el->rightend;
    el->rightend->left         = el->leftend;
    el->rightend->right        = NULL;
    el->hash[0]                = el->leftend;
    el->hash[el->hashSize - 1] = el->rightend;
    return 0;
  }
  return 1;
}

static CT_VOVertex *provide_next_site(CT_Voronoi *vor,
                                      const CT_Vec2f *sites,
                                      size_t num) {
  CT_VOVertex *v = NULL;
  if (vor->numSites < num) {
    v       = ct_mpool_alloc(&vor->vpool);
    v->pos  = sites[vor->numSites++];
    v->refs = 0;
    set_vertex_id(vor, v);
  }
  return v;
}

///// public functions

ct_export int ct_voronoi_init(CT_Voronoi *vor, size_t num) {
  return (ct_mpool_init(&vor->vpool, 2 * num, sizeof(CT_VOVertex)) ||
          ct_mpool_init(&vor->epool, num, sizeof(CT_VOEdge)) ||
          edgelist_init(&vor->halfEdges, 2 * num) ||
          heap_init(&vor->heap, num));
}

ct_export void ct_voronoi_free(CT_Voronoi *vor) {
  ct_mpool_free(&vor->vpool);
  ct_mpool_free(&vor->epool);
  ct_mpool_free(&vor->halfEdges.pool);
  free(vor->halfEdges.hash);
  free(vor->heap.hash);
}

// TODO reset mpools
ct_export void ct_voronoi_compute(CT_Voronoi *vor,
                                  CT_Vec2f *sites,
                                  size_t num,
                                  CT_VOEdgeHandler handler,
                                  void *state) {
  CT_Vec2f min, max, candidate;
  CT_VOVertex *newsite, *p;
  CT_VOEdgeList *el = &vor->halfEdges;
  CT_VOHeap *heap   = &vor->heap;

  ct_bounds2fp((float *)sites, num, 2, &min, &max);
  qsort(sites, num, sizeof(CT_Vec2f), ct_compare2fv_yx);
  el->minx     = min.x;
  el->deltax   = max.x - min.x;
  heap->miny   = min.y;
  heap->deltay = max.y - min.y;

  vor->numVertices = 0;
  vor->numEdges    = 0;
  vor->numSites    = 0;
  vor->first       = provide_next_site(vor, sites, num);
  newsite          = provide_next_site(vor, sites, num);

  while (1) {
    if (!heap_is_empty(heap)) {
      candidate = heap_get_min(heap);
    }

    if (newsite &&
        (heap_is_empty(heap) || newsite->pos.y < candidate.y ||
         (newsite->pos.y == candidate.y && newsite->pos.x < candidate.x))) {
      // new site is smallest
      CT_VOHalfEdge *lbnd     = edgelist_leftbnd(el, &newsite->pos);
      CT_VOHalfEdge *rbnd     = lbnd->right;
      CT_VOVertex *bot        = rightreg(lbnd, vor->first);
      CT_VOEdge *e            = bisect(vor, bot, newsite);
      CT_VOHalfEdge *bisector = make_halfedge(el, e, 0);
      edgelist_insert(lbnd, bisector);
      if ((p = intersect(vor, lbnd, bisector))) {
        heap_delete(heap, lbnd, &vor->vpool);
        heap_insert(heap, lbnd, p, ct_dist2fv(&p->pos, &newsite->pos));
      }
      lbnd     = bisector;
      bisector = make_halfedge(el, e, 1);
      edgelist_insert(lbnd, bisector);
      if ((p = intersect(vor, bisector, rbnd))) {
        heap_insert(heap, bisector, p, ct_dist2fv(&p->pos, &newsite->pos));
      }
      newsite = provide_next_site(vor, sites, num);
    } else if (!heap_is_empty(heap)) {
      // isec is smallest
      CT_VOHalfEdge *lbnd  = heap_extract_min(heap);
      CT_VOHalfEdge *llbnd = lbnd->left;
      CT_VOHalfEdge *rbnd  = lbnd->right;
      CT_VOHalfEdge *rrbnd = rbnd->right;
      CT_VOVertex *bot     = leftreg(lbnd, vor->first);
      CT_VOVertex *top     = rightreg(rbnd, vor->first);
      CT_VOVertex *v       = set_vertex_id(vor, lbnd->vertex);
      edge_set_endpoint(vor, lbnd->parent, lbnd->side, v, handler, state);
      edge_set_endpoint(vor, rbnd->parent, rbnd->side, v, handler, state);
      halfedge_delete(lbnd);
      heap_delete(heap, rbnd, &vor->vpool);
      halfedge_delete(rbnd);
      int side = 0;
      if (bot->pos.y > top->pos.y) {
        CT_VOVertex *temp = bot;
        bot               = top;
        top               = temp;
        side              = 1;
      }
      CT_VOEdge *e            = bisect(vor, bot, top);
      CT_VOHalfEdge *bisector = make_halfedge(el, e, side);
      edgelist_insert(llbnd, bisector);
      edge_set_endpoint(vor, e, 1 - side, v, handler, state);
      dec_ref(&vor->vpool, v);
      if ((p = intersect(vor, llbnd, bisector))) {
        heap_delete(heap, llbnd, &vor->vpool);
        heap_insert(heap, llbnd, p, ct_dist2fv(&p->pos, &bot->pos));
      }
      if ((p = intersect(vor, bisector, rrbnd))) {
        heap_insert(heap, bisector, p, ct_dist2fv(&p->pos, &bot->pos));
      }
    } else
      break;
  }
  CT_VOHalfEdge *he = vor->halfEdges.leftend->right;
  while (he != vor->halfEdges.rightend) {
    handler(he->parent, state);
    he = he->right;
  }
}

ct_export void ct_vovertex_trace(const CT_VOVertex *v) {
  CT_INFO("id: %u (%p) pos: %f,%f refs: %u", v->id, v, v->pos.x, v->pos.y,
          v->refs);
}

ct_export void ct_vohedge_trace(const CT_VOHalfEdge *e) {
  CT_INFO("he: (%p) v: %p n: %p l: %p r: %p e: %p refs: %u", e, e->vertex,
          e->next, e->left, e->right, e->parent, e->refs);
}
