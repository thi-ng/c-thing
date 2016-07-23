#include "test/test.h"

#include "data/vector.h"

CT_TEST_DECLS

int test_vector() {
  CT_Vector a;
  CT_VectorIter i;
  float data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  size_t num   = sizeof(data) / sizeof(float);
  CT_IS(!ct_vector_init(&a, 8, sizeof(float)), "init");
  CT_IS(0 == ct_vector_size(&a), "not empty");

  i = ct_vector_iter_init(&a, 0);
  CT_IS(!ct_vector_iter_next(&i), "next != NULL");

  for (size_t i = 0; i < num; i++) {
    CT_IS(!ct_vector_push(&a, &data[i]), "push %zu", i);
    CT_IS(data[i] == *((float *)ct_vector_get(&a, i)), "get");
  }
  CT_IS(num == ct_vector_size(&a), "size != %zu", num);

  i = ct_vector_iter_init(&a, 0);
  float *val, last_val;
  while ((val = (float *)ct_vector_iter_next(&i))) {
    *val     = *val * 10;
    last_val = *val;
    CT_DEBUG("iter: %f", *val);
  }
  CT_IS(data[num - 1] * 10 == last_val, "last val");

  i = ct_vector_iter_init(&a, 1);
  while ((val = (float *)ct_vector_iter_prev(&i))) {
    *val     = *val * 10;
    last_val = *val;
    CT_DEBUG("riter: %f", *val);
  }
  CT_IS(data[0] * 100 == last_val, "last val");

  float pop;
  while (!ct_vector_pop(&a, &pop)) {
    CT_DEBUG("pop: %f", pop);
  }
  CT_IS(0 == ct_vector_size(&a), "not empty");

  return 0;
}
