#include <stdio.h>
#include "config.h"

extern void test_mpool();
extern void test_vec2f();
extern void test_vec3f();
extern void test_swizzle();

int main() {
#ifdef CT_USE_SSE
    printf("using SSE...\n");
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
}
