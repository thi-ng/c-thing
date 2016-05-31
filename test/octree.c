#include "dbg.h"
#include "test.h"

#include "octree.h"

CT_TEST_DECLS

struct bounds_t {
  CT_Vec3f min, max;
  size_t num;
};

int ct_octree_bounds(CT_Octree *q, void *state) {
  struct bounds_t *bounds = (struct bounds_t *)state;
  ct_min3fv_imm(&bounds->min, q->point);
  ct_max3fv_imm(&bounds->max, q->point);
  bounds->num++;
  return 0;
}

int test_octree() {
  CT_INFO("CT_Octree size: %zd", sizeof(CT_Octree));
  CT_Octree q;
  CT_MPool qpool, vpool;
  ct_octree_init(&q, 0, 0, 0, 100, 100, 100);
  ct_mpool_init(&qpool, 0x10000, sizeof(CT_Octree));
  ct_mpool_init(&vpool, 0x10000, sizeof(CT_Vec3f));
  CT_Vec3f *a = ct_vec3f(10, 10, 10, &vpool);
  CT_Vec3f *b = ct_vec3f(10, 11, 10, &vpool);
  CT_Vec3f *b2 = ct_vec3f(10.1, 11, 10, &vpool);
  CT_Vec3f *c = ct_vec3f(50, 12, 50, &vpool);
  ct_octree_insert(&q, a, NULL, &qpool);
  ct_octree_insert(&q, b, NULL, &qpool);
  ct_octree_insert(&q, c, NULL, &qpool);
  ct_octree_trace(&q, 0);
  CT_IS(ct_octree_find_leaf(&q, a), "can't find a");
  CT_IS(ct_octree_find_leaf(&q, b), "can't find b");
  CT_IS(!ct_octree_find_leaf(&q, b2), "shouldn't find b2");
  ct_octree_remove(&q, a, &qpool);
  CT_IS(!ct_octree_find_leaf(&q, a), "shouldn't find a");
  CT_IS(ct_octree_remove(&q, a, &qpool), "remove a again");
  ct_octree_remove(&q, b, &qpool);
  CT_IS(!ct_octree_find_leaf(&q, b), "shouldn't find b");
  CT_IS(ct_octree_remove(&q, b, &qpool), "remove b again");
  ct_octree_remove(&q, c, &qpool);
  CT_IS(!ct_octree_find_leaf(&q, c), "shouldn't find c");
  CT_IS(ct_octree_remove(&q, c, &qpool), "remove c again");
  CT_IS(CT_TREE_EMPTY == q.type, "root is not empty: %zd", q.type);
  //srand(time(0));
  int num = 1e5;
  for (int i = 0; i < num; i++) {
    CT_Vec3f *p = ct_vec3f(ct_rand_normpos() * 100, ct_rand_normpos() * 100,
                           ct_rand_normpos() * 100, &vpool);
    ct_octree_insert(&q, p, NULL, &qpool);
  }
  struct bounds_t bounds = {{1000, 1000, 1000}, {-1000, -1000, -1000}, 0};
  ct_octree_visit_leaves(&q, ct_octree_bounds, &bounds);
  CT_IS(num == bounds.num, "wrong leaf count: %zd", bounds.num);
  CT_INFO("%f,%f,%f -> %f,%f,%f %zd", bounds.min.x, bounds.min.y, bounds.min.z,
          bounds.max.x, bounds.max.y, bounds.max.z, bounds.num);
  //ct_octree_trace(&q, 0);
  ct_mpool_trace(&qpool);
  ct_mpool_free_all(&vpool);
  ct_mpool_free_all(&qpool);
  return 0;
}
