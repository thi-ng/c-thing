#include "data/cons.h"
#include "common/dbg.h"
#include "common/test.h"
#include "mem/mpool.h"

CT_TEST_DECLS

typedef struct {
  size_t count;
  size_t err;
  void *ref;
} ConsTestState;

static void trace_cons(CT_Cons *c, void *_) {
  CT_INFO("cell: %p, val: %s, next: %p", c, (char *)c->value, c->next);
}

static void trace_consf(CT_Cons *c, void *_) {
  CT_INFO("cell: %p, val: %f, next: %p", c, *((float *)c->value), c->next);
}

static void count_cons(CT_Cons *c, void *state) {
  //trace_cons(c, NULL);
  ((ConsTestState *)state)->count++;
}

static void verify_cons_char(CT_Cons *c, void *state) {
  ConsTestState *cc = (ConsTestState *)state;
  if ((char *)c->value == ((char **)cc->ref)[cc->count]) {
    cc->count++;
  }
}

static void verify_cons_float(CT_Cons *c, void *state) {
  ConsTestState *cc = (ConsTestState *)state;
  if ((float *)c->value == (float *)cc->ref + cc->count) {
    cc->count++;
  }
}

static ConsTestState *reset_state(ConsTestState *cs, void *ref) {
  cs->count = cs->err = 0;
  cs->ref             = ref;
  return cs;
}

int test_cons() {
  CT_Cons *c =
      ct_cons("CCC", ct_cons("BBB", ct_cons("AAA", NULL, NULL), NULL), NULL);
  ConsTestState cs = {.count = 0, .err = 0};
  ct_cons_iterate(c, count_cons, &cs);
  CT_IS(cs.count == 3, "c length != 3 (%zu)", cs.count);
  ct_cons_free_all(c, 0);

  CT_MPool pool;
  ct_mpool_init(&pool, 256, sizeof(CT_Cons));
  c = ct_cons("CCC", ct_cons("BBB", ct_cons("AAA", NULL, &pool), &pool), &pool);
  reset_state(&cs, NULL);
  ct_cons_iterate(c, count_cons, &cs);
  CT_IS(cs.count == 3, "c mpool length != 3 (%zu)", cs.count);

  char *vals[] = {"DDD", "EEE", "FFF"};
  CT_Cons *c2  = ct_cons_from_parray((void **)vals, 3, NULL, &pool);
  ct_cons_iterate(c2, verify_cons_char, reset_state(&cs, vals));
  CT_IS(!cs.err, "cons != src array (%zu errors)", cs.err);
  CT_IS(cs.count == 3, "c2 length != 3 (%zu)", cs.count);

  float fvals[] = {23, 24, 25};
  CT_Cons *cf   = ct_cons_from_array(fvals, 3, sizeof(float), NULL, &pool);
  reset_state(&cs, fvals);
  ct_cons_iterate(cf, verify_cons_float, &cs);
  CT_IS(!cs.err, "cons != src array (%zu errors)", cs.err);
  CT_IS(cs.count == 3, "c2 length != 3 (%zu)", cs.count);

  ct_cons_iterate(ct_cons_concat_imm(c, c2), count_cons,
                  reset_state(&cs, NULL));
  CT_IS(cs.count == 6, "concat_imm(c,c2) length != 6 (%zu)", cs.count);

  CT_Cons *c3 = ct_cons_concat(c, c, &pool);
  ct_cons_iterate(c3, count_cons, reset_state(&cs, NULL));
  CT_IS(cs.count == 12, "concat(c,c) length != 12 (%zu)", cs.count);

  ct_cons_iterate(c2, verify_cons_char, reset_state(&cs, vals));
  CT_IS(!cs.err, "cons != src array (%zu errors)", cs.err);
  CT_IS(cs.count == 3, "c2 length != 3 (%zu)", cs.count);

  ct_cons_iterate(ct_cons_take(ct_cons_concat_imm(c, c), 20, &pool), count_cons,
                  reset_state(&cs, NULL));

  ct_mpool_free_all(&pool);
  return 0;
}
