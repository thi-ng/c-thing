#pragma once

#include <stddef.h>

#include "config.h"
#include "mem/mem.h"

CT_BEGIN_DECLS

typedef struct CT_Ref CT_Ref;

struct CT_Ref {
  void (*free)(const CT_Ref *);
  size_t count;
};

ct_inline void ct_ref_inc(const CT_Ref *ref) {
  ((CT_Ref *)ref)->count++;
}

ct_inline void ct_ref_dec(const CT_Ref *ref) {
  if (--((CT_Ref *)ref)->count == 0) ref->free(ref);
}

CT_END_DECLS
