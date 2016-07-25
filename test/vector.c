#include "test/test.h"

#include "data/vector.h"

CT_TEST_DECLS

int test_vector() {
  float data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  size_t num   = sizeof(data) / sizeof(float);

  CT_Vector *v = ct_vector_new(8, sizeof(float));
  CT_IS(v, "init");
  CT_IS(0 == ct_vector_size(v), "not empty");

  CT_VectorIter *iter = ct_vector_iter_new(v, 0);
  CT_IS(iter, "init");
  CT_IS(!ct_vector_iter_next(iter), "next != NULL");

  for (size_t i = 0; i < num; i++) {
    CT_IS(!ct_vector_push(v, &data[i]), "push %zu", i);
    CT_IS(data[i] == *((float *)ct_vector_get(v, i)), "get");
  }
  CT_IS(num == ct_vector_size(v), "size != %zu", num);

  ct_vector_iter_init(iter, v, 0);
  float *val, last_val;
  while ((val = (float *)ct_vector_iter_next(iter))) {
    *val     = *val * 10;
    last_val = *val;
    CT_DEBUG("next: %f", *val);
  }
  CT_IS(data[num - 1] * 10 == last_val, "last val");

  ct_vector_iter_init(iter, v, 1);
  while ((val = (float *)ct_vector_iter_next(iter))) {
    CT_DEBUG("p: %p", val);
    *val     = *val * 10;
    last_val = *val;
    CT_DEBUG("rnext: %f", *val);
  }
  CT_IS(data[0] * 100 == last_val, "last val");

  while ((val = (float *)ct_vector_iter_prev(iter))) {
    CT_DEBUG("p: %p", val);
    *val     = *val * 10;
    last_val = *val;
    CT_DEBUG("rprev: %f", *val);
  }
  CT_IS(data[num - 1] * 1000 == last_val, "last val");

  float pop;
  while (!ct_vector_pop(v, &pop)) {
    CT_DEBUG("pop: %f", pop);
  }
  CT_IS(0 == ct_vector_size(v), "not empty");

  ct_vector_free(v);
  free(iter);
  return 0;
fail:
  return 1;
}
