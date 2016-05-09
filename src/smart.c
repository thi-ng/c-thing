#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

typedef struct {
    void *ptr;
    uint32_t count;
} smart_ptr;

smart_ptr *alloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL)
        return NULL;
    smart_ptr *sp = (smart_ptr *)malloc(sizeof(smart_ptr));
    if (sp != NULL) {
        sp->ptr = ptr;
        sp->count = 1;
    }
    return sp;
}

void trace_sp(smart_ptr *sp) {
    printf("%p->%p (%u)", sp, sp->ptr, sp->count);
}
