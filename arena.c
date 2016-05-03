#include "arena.h"

#ifdef TRACE_ARENA
#include <stdio.h>
#endif

CT_Arena *ct_arena(size_t size) {
  CT_Arena *arena = (CT_Arena*)malloc(sizeof(CT_Arena));
  if (arena != NULL) {
    uint8_t *pool = (uint8_t*)malloc(size);
    if (arena != NULL) {
      arena->pool = pool;
      arena->size = size;
      arena->pos = 0;
    } else {
      free(arena);
      arena = NULL;
    }
  }
  return arena;
}

void *ct_arena_malloc(CT_Arena *arena, size_t size) {
  if (arena->pos + size <= arena->size) {
    #ifdef TRACE_ARENA
      printf("alloc arena: %lu (%lu)\n", arena->pos, size);
    #endif
    void *ptr = &arena->pool[arena->pos];
    arena->pos += size + 3;
    arena->pos &= -4;
    return ptr;
  } else {
    return NULL;
  }
}

void ct_arena_free(CT_Arena *arena) {
  free(arena->pool);
  free(arena);
}
