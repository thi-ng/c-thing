#include "test/test.h"

#include "data/octree.h"

CT_TEST_DECLS

struct bounds_t {
  CT_Vec3f min, max;
  size_t num;
};

int ct_octree_bounds(const CT_OTNode *node, void *state) {
  struct bounds_t *bounds = (struct bounds_t *)state;
  ct_min3fv_imm(&bounds->min, node->point);
  ct_max3fv_imm(&bounds->max, node->point);
  bounds->num++;
  return 0;
}

int test_octree() {
  CT_INFO("CT_Octree size: %zu", sizeof(CT_Octree));
  CT_INFO("CT_OTNode size: %zu", sizeof(CT_OTNode));
  CT_Octree t;
  CT_MPool vpool;
  ct_octree_init(&t, 0, 0, 0, 100, 100, 100, 0x10000);
  ct_mpool_init(&vpool, 0x10000, sizeof(CT_Vec3f));
  CT_Vec3f *a  = ct_vec3f(10, 10, 10, &vpool);
  CT_Vec3f *b  = ct_vec3f(10, 11, 10, &vpool);
  CT_Vec3f *b2 = ct_vec3f(10.1, 11, 10, &vpool);
  CT_Vec3f *c  = ct_vec3f(50, 12, 50, &vpool);
  ct_octree_insert(&t, a, NULL);
  ct_octree_insert(&t, b, NULL);
  ct_octree_insert(&t, c, NULL);
  //ct_octree_trace(&t);
  CT_IS(ct_octree_find_leaf(&t, a), "can't find a");
  CT_IS(ct_octree_find_leaf(&t, b), "can't find b");
  CT_IS(!ct_octree_find_leaf(&t, b2), "shouldn't find b2");
  ct_octree_remove(&t, a);
  CT_IS(!ct_octree_find_leaf(&t, a), "shouldn't find a");
  CT_IS(ct_octree_remove(&t, a), "remove a again");
  ct_octree_remove(&t, b);
  CT_IS(!ct_octree_find_leaf(&t, b), "shouldn't find b");
  CT_IS(ct_octree_remove(&t, b), "remove b again");
  ct_octree_remove(&t, c);
  CT_IS(!ct_octree_find_leaf(&t, c), "shouldn't find c");
  CT_IS(ct_octree_remove(&t, c), "remove c again");
  CT_IS(CT_TREE_EMPTY == t.root.type, "root is not empty: %zu", t.root.type);
  //srand(time(0));
  int num = 1e5;
  for (int i = 0; i < num; i++) {
    CT_Vec3f *p = ct_vec3f(ct_rand_normpos() * 100, ct_rand_normpos() * 100,
                           ct_rand_normpos() * 100, &vpool);
    ct_octree_insert(&t, p, NULL);
  }
  struct bounds_t bounds = {{1000, 1000, 1000}, {-1000, -1000, -1000}, 0};
  ct_octree_visit_leaves(&t, ct_octree_bounds, &bounds);
  CT_IS(num == bounds.num, "wrong leaf count: %zu", bounds.num);
  CT_INFO("%f,%f,%f -> %f,%f,%f %zu", bounds.min.x, bounds.min.y, bounds.min.z,
          bounds.max.x, bounds.max.y, bounds.max.z, bounds.num);
  //ct_octree_trace(&t);
  ct_mpool_free(&vpool);
  //ct_mpool_trace(&t.pool);
  ct_octree_free(&t);
  return 0;
}
