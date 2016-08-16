#include "ct-head/random.h"
#include "ct-head/test.h"

CT_TEST_DECLS

#define NUM 16

int test_random() {
  CT_Smush rnd;
  uint32_t expected[NUM] = {0x00000000, 0x9a2343da, 0xdb354663, 0x72f2735c,
                            0x9cf0caa5, 0x4509caf1, 0x875199db, 0x20a6a896,
                            0xe95ab6aa, 0x726b4880, 0xad879015, 0xa771cf8c,
                            0xac3ec833, 0xd78b95ff, 0x88aae504, 0x5490a69b};
  ct_smush_init(&rnd, 0);
  for (size_t i = 0; i < NUM; i++) {
    uint32_t x = ct_smush(&rnd);
    CT_IS(x == expected[i], "rnd %zu: %08x", i, x);
  }
  return 0;
}

#undef NUM
