#include "mpool.h"
#include <stdio.h>
#include <assert.h>

int main() {
    CT_MPool pool;
    ct_mpool_init(&pool, 16, 8);
    ct_mpool_trace(&pool);
    float *mpool = (float *)ct_mpool_malloc(&pool);
    *mpool = 23;
    printf("mpool=%p, %f\n", mpool, *mpool);
    ct_mpool_trace(&pool);
    float *b = (float *)ct_mpool_malloc(&pool);
    *b = 42;
    printf("b=%p, %f\n", b, *b);
    ct_mpool_trace(&pool);
    ct_mpool_free(&pool, mpool);
    printf("free mpool\n");
    ct_mpool_trace(&pool);
    mpool = (float *)ct_mpool_malloc(&pool);
    *mpool = 66;
    printf("mpool=%p, %f\n", mpool, *mpool);
    ct_mpool_trace(&pool);
    float *c = (float *)ct_mpool_malloc(&pool);
    *c = -1;
    printf("c=%p, %f\n", c, *c);
    ct_mpool_trace(&pool);
    ct_mpool_free(&pool, c);
    ct_mpool_free(&pool, mpool);
    ct_mpool_free(&pool, b);
    ct_mpool_trace(&pool);
    ct_mpool_free_all(&pool);
    return 0;
}
