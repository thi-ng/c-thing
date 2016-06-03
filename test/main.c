#include "config.h"
#include "dbg.h"
#include "test.h"

extern int test_mpool();
extern int test_mpool_resize();
extern int test_vec2f();
extern int test_vec3f();
extern int test_swizzle();
extern int test_vec_hash();
extern int test_circle();
extern int test_qedge();
extern int test_cons();
extern int test_consrc();
extern int test_hash_fns();
extern int test_quadtree();
extern int test_octree();
extern int test_hashtable_char();
extern int test_hashtable_vec();

extern int bench_vec3_create();
extern int bench_hashtable();

CT_TEST_DECLS

int all_tests() {
#ifdef CT_FEATURE_CHECKS
  CT_INFO("using checks...");
#endif
#ifdef CT_FEATURE_TRACE_MPOOL
  CT_INFO("tracing mpool...");
#endif
#ifdef CT_FEATURE_SSE
  CT_INFO("using SSE...");
#endif
  CT_RUN_TEST(test_mpool);
  CT_RUN_TEST(test_mpool_resize);
  CT_RUN_TEST(test_vec2f);
  CT_RUN_TEST(test_vec3f);
  CT_RUN_TEST(bench_vec3_create);
  CT_RUN_TEST(test_swizzle);
  CT_RUN_TEST(test_vec_hash);
  CT_RUN_TEST(test_circle);
  CT_RUN_TEST(test_qedge);
  CT_RUN_TEST(test_cons);
  CT_RUN_TEST(test_consrc);
  CT_RUN_TEST(test_hash_fns);
  CT_RUN_TEST(test_quadtree);
  CT_RUN_TEST(test_octree);
  CT_RUN_TEST(test_hashtable_char);
  CT_RUN_TEST(test_hashtable_vec);
  CT_RUN_TEST(bench_hashtable);
  return 0;
}

CT_RUN_TESTS(all_tests)
