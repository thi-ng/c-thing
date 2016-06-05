#include <stdio.h>

#include "common/dbg.h"
#include "common/test.h"
#include "io/svg.h"
#include "math/math.h"
#include "math/poisson.h"

CT_TEST_DECLS

#ifndef __EMSCRIPTEN__
#include "elaine.h"

static uint32_t w = 512;
static uint32_t h = 512;

static float disc_gen(CT_Quadtree *t, CT_Circle2f *disc) {
  ct_set2fxy(&disc->pos, ct_rand_normpos() * t->root.w,
             ct_rand_normpos() * t->root.h);
  //return disc->r = (sinf(disc->pos.x * 0.04f) * cosf(disc->pos.y * 0.005f + PI)) * 10 + 11.5f;
  //return disc->r = ct_mixf(4, 50, ct_dist2fv(&t->root.centroid, &disc->pos) / 600.0);
  size_t x       = (size_t)disc->pos.x;
  size_t y       = (size_t)disc->pos.y;
  float tt       = 1.0f - (float)elaine_gray8[y * w + x] / 255.f;
  return disc->r = ct_mixf(1.5f, 8.f, powf(tt, 3.f));
}

int test_poisson_svg() {
  CT_Quadtree t;
  CT_MPool vpool;
  CT_IS(!ct_qtree_init(&t, 0, 0, w, h, 0x4000), "init");
  CT_IS(!ct_mpool_init(&vpool, 0x4000, sizeof(CT_Vec2f)), "init pool");
  FILE *f = fopen("poisson.svg", "w");
  char buf[0x10000];
  setvbuf(f, buf, _IOFBF, 0x4000);
  ct_svg_write_header(
      f, ct_svg_attribs(1, 2, SVG_INT("width", w), SVG_INT("height", h)));
  ct_svg_write_rect(f, 0, 0, w, h, ct_svg_attribs(1, 1, SVG_HEX("fill", 0xff)));
  fprintf(f, "<g stroke=\"none\" fill=\"white\">\n");
  srand(time(0));
  size_t count  = 0;
  size_t failed = 0;
  CT_Vec2f *p   = (CT_Vec2f *)ct_mpool_alloc(&vpool);
  while (1) {
    if (!ct_poisson_sample2f_with(&t, disc_gen, 5, p)) {
      ct_svg_write_circle(f, p->x, p->y, 1.f, NULL);
      p = (CT_Vec2f *)ct_mpool_alloc(&vpool);
      count++;
      failed = 0;
    } else if (++failed > 1e2) {
      break;
    }
  }
  fprintf(f, "</g></svg>");
  fclose(f);
  CT_INFO("%zu points", count);
  ct_mpool_free_all(&vpool);
  ct_qtree_free(&t);
  return 0;
}
#endif

int test_poisson() {
  CT_Quadtree t;
  CT_MPool vpool;
  int w = 600, h = 600;
  CT_IS(!ct_qtree_init(&t, 0, 0, w, h, 0x1000), "init");
  CT_IS(!ct_mpool_init(&vpool, 0x1000, sizeof(CT_Vec2f)), "init pool");
  srand(0);
  float r       = 10;
  size_t count  = 0;
  size_t failed = 0;
  CT_Vec2f *p   = (CT_Vec2f *)ct_mpool_alloc(&vpool);
  while (1) {
    if (!ct_poisson_sample2f(&t, r, 20, p)) {
      p = (CT_Vec2f *)ct_mpool_alloc(&vpool);
      count++;
      failed = 0;
    } else if (++failed > 50) {
      break;
    }
  }
  CT_IS(2000 < count, "count: %zu", count);
  ct_mpool_free_all(&vpool);
  ct_qtree_free(&t);
  return 0;
}
