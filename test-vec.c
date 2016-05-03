#include <stdio.h>
#include <assert.h>

#include "arena.h"
#include "vec.h"

void assert_vec2f(CT_Vec2f *v, float x, float y) {
  assert(v->x == x);
  assert(v->y == y);
}

void assert_vec3f(CT_Vec3f *v, float x, float y, float z) {
  assert(v->x == x);
  assert(v->y == y);
  assert(v->z == z);
}


int main() {
  CT_Arena *pool = ct_arena(1024);
  CT_Vec2f *a = ct_vec2f(1, 2, pool);
  CT_Vec2f *b = ct_add2fn(a, 10, pool);
  assert_vec2f(b, 11, 12);
  CT_Vec2f *c = ct_add2fv(a, b, pool);
  assert_vec2f(c, 12, 14);
  CT_Vec2f *d = ct_add2fv(a, b, NULL);
  assert_vec2f(d, 12, 14);
  ct_add2fv_imm(d, a);
  assert_vec2f(d, 13, 16);
  ct_add2fn_imm(d, 100);
  assert_vec2f(d, 113, 116);
  ct_add2fn_imm(d, -100);
  assert_vec2f(d, 13, 16);
  ct_add2fxy_imm(d, 10, 20);
  assert_vec2f(d, 23, 36);
  CT_Vec2f *e = ct_add2fxy(a, 10, 20, pool);
  assert_vec2f(e, 11, 22);
  assert(11 * 11 + 22 * 22 == ct_dot2fv(e, e));
  
  free(d);
  ct_arena_free(pool);
  return 0;
}
