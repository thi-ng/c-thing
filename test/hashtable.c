#include "hashtable.h"
#include "hashfn.h"
#include "test.h"

CT_TEST_DECLS

struct htest_t {
  size_t num;
};

static int val_is(CT_Hashtable *t, char *k, char *v) {
  char *res = (char *)ct_ht_get(t, k, strlen(k));
  return res ? !strcmp(res, v) : 0;
}

static int dump_ht(CT_HTEntry *e, void *state) {
  struct htest_t *s = (struct htest_t *)state;
  CT_INFO("entry: %zd, %s", s->num, e->val);
  s->num++;
  return 0;
}

static size_t visit_count(CT_Hashtable *t) {
  struct htest_t v = {.num = 0};
  ct_ht_iterate(t, dump_ht, &v);
  return v.num;
}

int test_hashtable() {
  CT_Hashtable t;
  CT_HTOps ops = {.hash = ct_murmur3_32};
  CT_IS(!ct_ht_init(&t, &ops, 4), "init error");
  CT_IS(!ct_ht_assoc(&t, "a", "aaa", 1), "assoc a");
  CT_IS(1 == t.numEntries, "size: %u", t.numEntries);
  CT_IS(val_is(&t, "a", "aaa"), "get a");
  CT_IS(!ct_ht_assoc(&t, "a", "AAA", 1), "re-assoc a");
  CT_IS(val_is(&t, "a", "AAA"), "get a");
  CT_IS(NULL == ct_ht_get(&t, "b", 1), "no b");
  // hash collissions
  CT_IS(!ct_ht_assoc(&t, "b", "bbb", 1), "assoc b");
  CT_IS(!ct_ht_assoc(&t, "c", "ccc", 1), "assoc c");
  CT_IS(!ct_ht_assoc(&t, "d", "ddd", 1), "assoc d");
  CT_IS(val_is(&t, "b", "bbb"), "get b");
  CT_IS(val_is(&t, "c", "ccc"), "get c");
  CT_IS(val_is(&t, "d", "ddd"), "get d");

  CT_IS(4 == visit_count(&t), "count");

  CT_IS(!ct_ht_dissoc(&t, "c", 1), "dissoc c");
  CT_IS(3 == visit_count(&t), "count");
  CT_IS(!ct_ht_dissoc(&t, "b", 1), "dissoc b");
  CT_IS(2 == t.numEntries, "numEntries %u", t.numEntries);
  CT_IS(!ct_ht_dissoc(&t, "d", 1), "dissoc d");
  CT_IS(1 == t.numEntries, "numEntries %u", t.numEntries);
  CT_IS(!ct_ht_dissoc(&t, "a", 1), "dissoc a");
  CT_IS(0 == t.numEntries, "numEntries %u", t.numEntries);
  CT_IS(ct_ht_dissoc(&t, "a", 1), "re-dissoc a");
  CT_IS(0 == t.numEntries, "numEntries %u", t.numEntries);
  ct_mpool_trace(&t.pool);
  ct_ht_free(&t);
  return 0;
}
