#include <stdlib.h>
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
  if (d != NULL) strcpy(d, s);
  return d;
}

int test_consrc() {
  CT_Object *a, *b, *c, *l, *l2;
  CT_IS(!ct_consrc_init(), "init");
  ct_object_assign(&a, ct_object_f32(23));
  ct_object_assign(&b, ct_object_i32(-42));
  ct_object_assign(&c, ct_object_str(strdup("foo"), 1));
  ct_object_assign(&l, ct_object_cons(c));
  CT_IS(ct_object_is(a, CT_TYPE_F32), "f32");
  CT_IS(ct_object_is(b, CT_TYPE_I32), "i32");
  CT_IS(ct_object_is(c, CT_TYPE_STR), "str");
  CT_IS(ct_object_is(l, CT_TYPE_CONS), "cons");
  ct_object_tostring(l, buf, BUF_SIZE);
  CT_IS(0 == strcmp("(\"foo\")", buf), "l: %s", buf);
  ct_consrc_push(&l, b);
  ct_consrc_push(&l, a);
  ct_consrc_push(&l, ct_object_str("bar", 0));
  ct_consrc_push(&l, &CT_NIL);
  ct_object_tostring(l, buf, BUF_SIZE);
  CT_IS(0 == strcmp("(nil \"bar\" 23.000000 -42 \"foo\")", buf), "l: %s", buf);
  ct_object_unassign(&a);
  ct_object_unassign(&b);
  ct_object_tostring(l, buf, BUF_SIZE);
  CT_IS(0 == strcmp("(nil \"bar\" 23.000000 -42 \"foo\")", buf), "l: %s", buf);
  ct_object_assign(&l2, ct_object_cons(l));
  ct_consrc_push(&l2, c);
  ct_consrc_push(&l2, c);
  ct_object_tostring(l2, buf, BUF_SIZE);
  CT_IS(
      0 == strcmp("(\"foo\" \"foo\" (nil \"bar\" 23.000000 -42 \"foo\"))", buf),
      "l2: %s", buf);
  ct_object_unassign(&l);
  ct_object_unassign(&c);
  ct_consrc_push(&l2, l2);
  ct_object_tostring(l2, buf, BUF_SIZE);
  CT_IS(0 == strcmp("((\"foo\" \"foo\" (nil \"bar\" 23.000000 -42 \"foo\")) "
                    "\"foo\" \"foo\" (nil \"bar\" 23.000000 -42 \"foo\"))",
                    buf),
        "l2: %s", buf);
  fprintf(stderr, "l2: ");
  ct_object_print(l2, stderr);
  fprintf(stderr, "\n");
  ct_object_unassign(&l2);
  return 0;
}
