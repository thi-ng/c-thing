#include "hashfn.h"
#include "dbg.h"

#define MURMUR_C1 ((uint32_t)0xcc9e2d51)
#define MURMUR_C2 ((uint32_t)0x1b873593)
#define MURMUR_H1 ((uint32_t)0xe6546b64)
#define MURMUR_H2 ((uint32_t)0x85ebca6b)
#define MURMUR_H3 ((uint32_t)0xc2b2ae35)

CT_EXPORT uint32_t ct_murmur3_32(const void *data, size_t numBytes) {
  if (data == NULL || numBytes == 0) return 0;

  const size_t numBlocks = numBytes >> 2;
  const uint32_t *blocks = (const uint32_t *)(data);
  const uint8_t *tail = (const uint8_t *)&data[numBlocks << 2];

  uint32_t h = 0;
  uint32_t k;
  for (size_t i = 0; i < numBlocks; i++) {
    k = blocks[i] * MURMUR_C1;
    k = (k << 15) | (k >> 17);
    k *= MURMUR_C2;

    h ^= k;
    h = (h << 13) | (h >> 19);
    h = (h * 5) + MURMUR_H1;
  }

  k = 0;
  switch (numBytes & 3) {
    case 3:
      k ^= tail[2] << 16;
    case 2:
      k ^= tail[1] << 8;
    case 1:
      k ^= tail[0];
      k *= MURMUR_C1;
      k = (k << 15) | (k >> 17);
      k *= MURMUR_C2;
      h ^= k;
  }

  h ^= numBytes;

  h ^= h >> 16;
  h *= MURMUR_H2;
  h ^= h >> 13;
  h *= MURMUR_H3;
  h ^= h >> 16;

  return h;
}
