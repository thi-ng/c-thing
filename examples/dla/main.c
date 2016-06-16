#include <stdlib.h>
#include <time.h>

#include "common/dbg.h"
#include "config.h"

#include "data/spatialgrid.h"
#include "io/svg.h"
#include "math/math.h"
#include "math/vec.h"

#define BUF_SIZE 0x400

typedef struct {
  CT_Vec3f *particles;
  CT_SpatialGrid accel;
  size_t length;
  size_t num;
  size_t width;
  size_t height;
  size_t seeds;
  float eps[2];
} DLA;

// clang-format off
static DLA dla = {
  .width  = 600,
  .height = 600,
  .seeds  = 16,
  .eps    = {25, 25}
};
// clang-format on

ct_inline CT_Vec3f *rand_particle(DLA *dla) {
  return ct_floor3f_imm(ct_set3fxyz(&dla->particles[dla->num++],
                                    ct_rand_normpos() * dla->width,
                                    ct_rand_normpos() * dla->height, 0));
}

static void add_particle(DLA *dla) {
  CT_Vec3f *res[BUF_SIZE];
  CT_Vec3f *p = rand_particle(dla);
  while (1) {
    size_t num = ct_spgrid_select2d(&dla->accel, (float *)p, dla->eps,
                                    (void **)&res, BUF_SIZE);
    if (num > 0) {
      float minD  = 1e9;
      CT_Vec3f *c = NULL;
      for (size_t i = 0; i < num; i++) {
        float d = ct_distsq2fv((CT_Vec2f *)p, (CT_Vec2f *)res[i]);
        if (d >= 1 && d < minD) {
          minD = d;
          c    = res[i];
        }
      }
      if (c) {
#ifdef CT_FEATURE_SSE4
        p->mmval = (p->mmval - c->mmval) *
                       _mm_rcp_ps(_mm_sqrt_ps(_mm_load1_ps(&minD))) +
                   c->mmval;
#else
        float t = 1.0f / sqrtf(minD);
        p->x    = c->x + t * (p->x - c->x);
        p->y    = c->y + t * (p->y - c->y);
#endif
        p->z = c->z;
        ct_spgrid_insert(&dla->accel, (float *)p, p);
        return;
      }
    }
    ct_set3fxyz(p, ct_rand_normpos() * dla->width,
                ct_rand_normpos() * dla->height, 0);
  };
}

int main() {
  dla.length    = dla.width * dla.height >> 2;
  dla.particles = calloc(dla.length, sizeof(CT_Vec3f));
  ct_spgrid_init(&dla.accel, (float[]){0, 0}, (float[]){dla.width, dla.height},
                 (size_t[]){60, 60}, 2, 0x8000);
  srand(time(0));
  int cols[50];
  for (int i = 0; i < dla.seeds; i++) {
    CT_Vec3f *p = rand_particle(&dla);
    p->z        = i;
    ct_spgrid_insert(&dla.accel, (float *)p, p);
    cols[i] = rand() & 0xffffff;
  }
  while (dla.num < dla.length) {
    add_particle(&dla);
  }
  ct_svg_write_header(stdout, ct_svg_attribs(1, 2, SVG_INT("width", dla.width),
                                             SVG_INT("height", dla.height)));
  fprintf(stdout, "<g stroke=\"none\">\n");
  for (size_t i = 0; i < dla.num; i++) {
    CT_Vec3f *p = &dla.particles[i];
    ct_svg_write_circle(stdout, p->x, p->y, 1,
                        ct_svg_attribs(1, 1, SVG_HEX("fill", cols[(int)p->z])));
  }
  fprintf(stdout, "</g></svg>");
  fprintf(stderr, "%zu particles", dla.num);
  return 0;
}
