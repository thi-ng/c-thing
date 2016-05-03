#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#define ALLOCATE_TYPE(type) \
  (type *)(arena != NULL ? ct_arena_malloc(arena, sizeof(type)) : malloc(sizeof(type)))

#define ALLOCATE_BYTES(bytes) \
  (arena != NULL ? ct_arena_malloc(arena, bytes) : malloc(bytes))

typedef struct {
  uint8_t *pool;
  size_t size;
  size_t pos;
} CT_Arena;

CT_Arena *ct_arena(size_t size);
void *ct_arena_malloc(CT_Arena *arena, size_t size);
void ct_arena_free(CT_Arena *arena);
