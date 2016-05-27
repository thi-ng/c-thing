#include "config.h"
#include "dbg.h"
#include "test.h"

extern int test_mpool();
extern int test_vec2f();
extern int test_vec3f();
extern int test_swizzle();
extern int test_circle();
extern int test_qedge();
extern int test_cons();
extern int test_consrc();
extern int test_hash_fns();
extern int test_quadtree();

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
  CT_RUN_TEST(test_vec2f);
  CT_RUN_TEST(test_vec3f);
  CT_RUN_TEST(test_swizzle);
  CT_RUN_TEST(test_circle);
  CT_RUN_TEST(test_qedge);
  CT_RUN_TEST(test_cons);
  CT_RUN_TEST(test_consrc);
  CT_RUN_TEST(test_hash_fns);
  CT_RUN_TEST(test_quadtree);
  return 0;
}

CT_RUN_TESTS(all_tests)
