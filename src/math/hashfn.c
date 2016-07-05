#include "math/hashfn.h"
#include "math/math.h"

#define MURMUR_C1 ((uint32_t)0xcc9e2d51)
#define MURMUR_C2 ((uint32_t)0x1b873593)
#define MURMUR_H1 ((uint32_t)0xe6546b64)
#define MURMUR_H2 ((uint32_t)0x85ebca6b)
#define MURMUR_H3 ((uint32_t)0xc2b2ae35)

ct_inline uint32_t mixK(uint32_t k) {
  return ct_rotl32(k * MURMUR_C1, 15) * MURMUR_C2;
}

ct_inline uint32_t mixH(uint32_t h, uint32_t k) {
  return ct_rotl32(h ^ k, 13) * 5 + MURMUR_H1;
}

ct_inline uint32_t fmix(uint32_t h, size_t len) {
  h ^= len;
  h ^= h >> 16;
  h *= MURMUR_H2;
  h ^= h >> 13;
  h *= MURMUR_H3;
  h ^= h >> 16;
  return h;
}

ct_export uint32_t ct_murmur3_32(const void *data, size_t numBytes) {
  if (data == NULL || numBytes == 0) return 0;

  const size_t numBlocks = numBytes >> 2;
  const uint32_t *blocks = (const uint32_t *)data;
  const uint8_t *tail    = (const uint8_t *)&blocks[numBlocks];

  uint32_t h = 0, k = 0;
  for (size_t i = 0; i < numBlocks; i++) {
    h = mixH(h, mixK(blocks[i]));
  }

  switch (numBytes & 3) {
    case 3:
      k ^= tail[2] << 16;
    case 2:
      k ^= tail[1] << 8;
    case 1:
      h ^= mixK(k ^ tail[0]);
  }

  return fmix(h, numBytes);
}
