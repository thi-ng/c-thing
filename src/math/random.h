#include "cthing.h"

typedef struct {
  uint32_t seed;
  uint32_t iter;
} CT_Smush;

// https://gist.github.com/voidqk/d112165a26b45244a65298933c0349a4

ct_inline uint32_t ct_smush(CT_Smush *rnd) {
  const uint32_t m = 0x5bd1e995;
  const uint32_t k = rnd->iter++ * m;
  rnd->seed        = (k ^ (k >> 24) ^ (rnd->seed * m)) * m;
  return rnd->seed ^ (rnd->seed >> 13);
}

ct_inline void ct_smush_init(CT_Smush *rnd, uint32_t s) {
  rnd->seed = s;
  rnd->iter = 0;
}
