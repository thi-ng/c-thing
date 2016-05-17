#include <string.h>

#include "hashfn.h"
#include "test.h"

CT_TEST_DECLS

int test_hash_fns() {
  uint32_t h;
  char *data = "abcdefghij";
  h = ct_murmur3_32(data, strlen(data));
  CT_IS(0x88927791 == h, "m3 fail: %x", h);
  h = ct_murmur3_32(NULL, 100);
  CT_IS(0 == h, "m3 fail NULL: %x", h);
  return 0;
}
