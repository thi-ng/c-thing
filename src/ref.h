#pragma once

#include <stddef.h>

#define container_of(ptr, type, member) \
  ((type *)((char *)(ptr)-offsetof(type, member)))

typedef struct CT_Ref CT_Ref;

struct CT_Ref {
  void (*free)(const CT_Ref *);
  size_t count;
};

static inline void ct_ref_inc(const CT_Ref *ref) {
  ((CT_Ref *)ref)->count++;
}

static inline void ct_ref_dec(const CT_Ref *ref) {
  if (--((CT_Ref *)ref)->count == 0) ref->free(ref);
}
