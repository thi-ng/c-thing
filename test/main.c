#include "config.h"
#include "dbg.h"

extern void test_mpool();
extern void test_vec2f();
extern void test_vec3f();
extern void test_swizzle();
extern void test_circle();
extern void test_qedge();

int main() {
#ifdef CT_FEATURE_SSE
  CT_INFO("using SSE...");
#endif
#ifndef NO_MPOOL
  test_mpool();
#endif
#ifndef NO_VEC2
  test_vec2f();
#endif
#ifndef NO_VEC
  test_vec3f();
#endif
#ifndef NO_SWIZZLE
  test_swizzle();
#endif
#ifndef NO_CIRCLE
  test_circle();
#endif
#ifndef NO_QEDGE
  test_qedge();
#endif
}
