#include <string.h>

#include "test/test.h"

#include "data/consrc.h"
#include "data/object.h"

#define BUF_SIZE 1024
static char buf[BUF_SIZE];

extern CT_Object CT_NIL;

CT_TEST_DECLS

char *strdup(const char *s) {
  char *d = malloc(strlen(s) + 1);
  if (d != NULL) {
    strcpy(d, s);
  }
  return d;
}

int test_consrc() {
  CT_Object *a, *b, *c, *d, *e, *l, *l2;
  CT_IS(!ct_consrc_init(), "init consrc");
  ct_object_assign(&a, ct_object_f32(23));
  ct_object_assign(&b, ct_object_i32(-42));
  ct_object_assign(&e, ct_object_u32(66));
  ct_object_assign(&c, ct_object_vec4(-1, 2, -3, 4));
  ct_object_assign(&d, ct_object_str(strdup("foo"), 1));
  ct_object_assign(&l, ct_object_cons(d));
  CT_IS(ct_object_is(a, CT_TYPE_F32), "f32");
  CT_IS(ct_object_is(b, CT_TYPE_I32), "i32");
  CT_IS(ct_object_is(c, CT_TYPE_VEC4), "vec4");
  CT_IS(ct_object_is(d, CT_TYPE_STR), "str");
  CT_IS(ct_object_is(e, CT_TYPE_U32), "u32");
  CT_IS(ct_object_is(l, CT_TYPE_CONS), "cons");
  ct_object_tostring(l, buf, BUF_SIZE);
  CT_IS(0 == strcmp("(\"foo\")", buf), "l: %s", buf);
  ct_consrc_push_imm(&l, e);
  ct_consrc_push_imm(&l, c);
  ct_consrc_push_imm(&l, b);
  ct_consrc_push_imm(&l, a);
  ct_consrc_push_imm(&l, ct_object_str("bar", 0));
  ct_consrc_push_imm(&l, &CT_NIL);
  ct_object_tostring(l, buf, BUF_SIZE);
  CT_IS(0 == strcmp("(nil \"bar\" 23.000000 -42 [-1.000000 2.000000 -3.000000 "
                    "4.000000] 66 \"foo\")",
                    buf),
        "l: %s", buf);
  ct_object_unassign(&a);
  ct_object_unassign(&b);
  ct_object_tostring(l, buf, BUF_SIZE);
  CT_IS(0 == strcmp("(nil \"bar\" 23.000000 -42 [-1.000000 2.000000 -3.000000 "
                    "4.000000] 66 \"foo\")",
                    buf),
        "l: %s", buf);
  ct_object_assign(&l2, ct_object_cons(l));
  ct_consrc_push_imm(&l2, d);
  ct_consrc_push_imm(&l2, d);
  ct_object_tostring(l2, buf, BUF_SIZE);
  CT_IS(0 == strcmp("(\"foo\" \"foo\" (nil \"bar\" 23.000000 -42 [-1.000000 "
                    "2.000000 -3.000000 4.000000] 66 \"foo\"))",
                    buf),
        "l2: %s", buf);
  ct_object_unassign(&l);
  ct_object_unassign(&d);
  ct_consrc_push_imm(&l2, l2);
  ct_object_tostring(l2, buf, BUF_SIZE);
  CT_IS(0 == strcmp("((\"foo\" \"foo\" (nil \"bar\" 23.000000 -42 [-1.000000 "
                    "2.000000 -3.000000 4.000000] 66 \"foo\")) "
                    "\"foo\" \"foo\" (nil \"bar\" 23.000000 -42 [-1.000000 "
                    "2.000000 -3.000000 4.000000] 66 \"foo\"))",
                    buf),
        "l2: %s", buf);
  fputs("l2: ", stderr);
  ct_object_print(l2, stderr);
  fputs("\n", stderr);
  ct_object_unassign(&l2);
  return 0;
}

int test_consrc2() {
  CT_Object *a, *b, *c, *l, *l2, *l3;
  CT_IS(!ct_consrc_init(), "init consrc");
  ct_object_assign(&a, ct_object_f32(23));
  ct_object_assign(&b, ct_object_u32(42));
  ct_object_assign(&c, ct_object_u32(66));
  ct_object_assign(&l, ct_object_cons(a));
  //ct_object_print(l, stderr);
  CT_IS(1 == l->rc.count, "l count");
  ct_object_assign(&l2, ct_consrc_cons(b, l));
  //ct_object_print(l2, stderr);
  CT_IS(1 == l2->rc.count, "l2 count");
  //ct_object_trace(l);
  //ct_object_trace(l2);
  ct_object_assign(&l3, ct_consrc_cons(c, l));
  //ct_object_print(l3, stderr);
  CT_IS(3 == l->rc.count, "l count");
  //ct_object_trace(l);
  //ct_object_trace(l2);
  //ct_object_trace(l3);
  CT_Object *tmp = l;
  ct_object_unassign(&l);
  CT_IS(2 == tmp->rc.count, "l count");
  //ct_object_print(l2, stderr);
  ct_object_assign(&l, ct_consrc_cons(c, l2));
  CT_IS(2 == l2->rc.count, "l2 count");
  CT_IS(1 == l->rc.count, "l count");
  //ct_object_print(l, stderr);
  //ct_object_print(l2, stderr);
  //ct_object_trace(l);
  //ct_object_trace(l2);
  //ct_object_trace(l3);
  CT_Object *tmp2 = l2;
  ct_object_unassign(&l2);
  CT_IS(1 == tmp2->rc.count, "l2 count");
  tmp2 = l3;
  ct_object_unassign(&l3);
  CT_IS(0 == tmp2->rc.count, "l3 count");
  //ct_object_trace(l);
  ct_object_unassign(&l);
  CT_IS(0 == tmp->rc.count, "l orig count");
  return 0;
}
