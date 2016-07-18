#include <string.h>

#include "data/hashtable.h"
#include "math/vec.h"

typedef struct { CT_Vec3f *a, *b; } CT_MeshEdge;

typedef struct {
  uint32_t *vertices;
  uint32_t numV;
} CT_MeshFace;

typedef struct {
  CT_Hashtable vertices;
  CT_Hashtable edges;
  CT_MPool vpool;
  uint32_t vid;
} CT_Mesh;

static uint32_t hash_vec3f(const void *v, size_t _) {
  return ct_murmur3_32(v, 12);
}

static int equiv_vec3f(const void *a, const void *b, size_t as, size_t bs) {
  return !memcmp(a, b, 12);
}

static uint32_t hash_edge_vec3f(const void *a, size_t _) {
  const CT_MeshEdge *e = (CT_MeshEdge *)a;
  return ct_murmur3_32(e->a, 12) + ct_murmur3_32(e->b, 12);
}

static int equiv_edge_vec3f(const void *a, const void *b, size_t as,
                            size_t bs) {
  CT_MeshEdge *ea = (CT_MeshEdge *)a;
  CT_MeshEdge *eb = (CT_MeshEdge *)b;
  if (!memcmp(ea->a, eb->a, 12)) {
    if (!memcmp(ea->b, eb->b, 12)) {
      return 1;
    }
  }
  if (!memcmp(ea->a, eb->b, 12)) {
    if (!memcmp(ea->b, eb->a, 12)) {
      return 1;
    }
  }
  return 0;
}

static CT_HTOps _ht_vertex_ops_vec3f = {.hash       = hash_vec3f,
                                        .equiv_keys = equiv_vec3f};

static CT_HTOps _ht_edge_ops_vec3f = {.hash       = hash_edge_vec3f,
                                      .equiv_keys = equiv_edge_vec3f};

int ct_mesh_init(CT_Mesh *m, size_t numF, size_t numV) {
  ct_ht_init(&m->vertices, &_ht_vertex_ops_vec3f, numV, numV >> 2, CT_HT_NONE);
  ct_ht_init(&m->edges, &_ht_edge_ops_vec3f, numF * 2, numF >> 1,
             CT_HT_CONST_KEYS | CT_HT_CONST_VALS);
  ct_mpool_init(&m->vpool, numV, sizeof(CT_Vec3f));
  m->vid = 0;
  return 0;
}

void ct_mesh_free(CT_Mesh *m) {
  ct_ht_free(&m->vertices);
  ct_ht_free(&m->edges);
}

int ct_mesh_add_face(CT_Mesh *m, CT_Vec3f **verts, size_t num) {
  uint32_t ids[4];
  uint32_t prev_id, id;
  for (size_t i = 0; i <= num; i++) {
    if (id < num) {
      uint32_t *vid = (uint32_t *)ct_ht_get(&m->vertices, verts[i], 12, NULL);
      if (!vid) {
        ct_ht_assoc(&m->vertices, verts[i], &m->vid, sizeof(CT_Vec3f), 4);
        id = m->vid;
        m->vid++;
      } else {
        id = *vid;
      }
      ids[i] = id;
    } else {
      id = ids[0];
    }
  }
  return 0;
}
