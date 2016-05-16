#include "cons.h"
#include "dbg.h"
#include "mpool.h"

void trace_cons(CT_Cons *c, void *_) {
  CT_INFO("cell: %p, val: %s, next: %p", c, (char *)c->value, c->next);
}

void test_cons() {
  CT_Cons *c =
      ct_cons("CCC", ct_cons("BBB", ct_cons("AAA", NULL, NULL), NULL), NULL);
  ct_cons_iterate(c, trace_cons, NULL);
  ct_cons_free(c, 0);
  
  CT_MPool pool;
  ct_mpool_init(&pool, 4, sizeof(CT_Cons));
  c = ct_cons("CCC", ct_cons("BBB", ct_cons("AAA", NULL, &pool), &pool), &pool);
  ct_cons_iterate(c, trace_cons, NULL);
  ct_mpool_free_all(&pool);
}
