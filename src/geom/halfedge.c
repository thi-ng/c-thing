// #pragma once

#include "common/dbg.h"
#include "math/vec.h"
#include "mem/mpool.h"

typedef struct CT_HalfEdge CT_HalfEdge;
typedef struct CT_HEVertex CT_HEVertex;
typedef struct CT_HEFace CT_HEFace;

struct CT_HalfEdge {
  CT_HEVertex *vertex;
  CT_HEFace *face;
  CT_HalfEdge *pair;
  CT_HalfEdge *next;
};

struct CT_HEVertex {
  CT_Vec3f pos;
  CT_HalfEdge *edge;
};

struct CT_HEFace {
  CT_HalfEdge *edge;
  CT_Vec3f normal;
};

typedef struct {
  CT_HEVertex **vertices;
  CT_HEFace **faces;
  CT_MPool mpoolV, mpoolF, mpoolE;
  size_t numV;
  size_t numF;
} CT_HEMesh;

static size_t _vertex_idx = 0;

CT_HEVertex *ct_hevertex(float *pos, CT_HalfEdge *e, CT_MPool *pool) {
  CT_HEVertex *v = CT_MP_ALLOC(pool, CT_HEVertex);
  CT_CHECK_MEM(v);
  ct_set3fxyz(&v->pos, pos[0], pos[1], pos[2]);
  v->edge = e;
fail:
  return v;
}

CT_EXPORT int ct_hemesh_init(CT_HEMesh *mesh, size_t numV, size_t numF) {
  ct_mpool_init(&mesh->mpoolV, numV, sizeof(CT_HEVertex));
  ct_mpool_init(&mesh->mpoolF, numF, sizeof(CT_HEFace));
  ct_mpool_init(&mesh->mpoolE, numF * 2, sizeof(CT_HalfEdge));
  mesh->vertices = calloc(numV, sizeof(CT_HEVertex *));
  mesh->faces = calloc(numF, sizeof(CT_HEFace *));
  mesh->numV = mesh->numF = 0;
  return 0;
}

CT_EXPORT void ct_hemesh_free(CT_HEMesh *mesh) {
  ct_mpool_free_all(&mesh->mpoolV);
  ct_mpool_free_all(&mesh->mpoolF);
  ct_mpool_free_all(&mesh->mpoolE);
}
