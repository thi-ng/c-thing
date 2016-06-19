#include <stdlib.h>
#include <float.h>
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
  return ct_set3fxyz(&dla->particles[dla->num++],
                     ct_rand_normpos() * dla->width,
                     ct_rand_normpos() * dla->height, 0);
}

static void add_particle(DLA *dla) {
  static CT_Vec3f *res[BUF_SIZE];
  CT_Vec3f *p = rand_particle(dla);
  while (1) {
    const size_t num = ct_spgrid_select2d(&dla->accel, (float *)p, dla->eps,
                                          (void **)&res, BUF_SIZE);
    if (num > 0) {
      float minD  = FLT_MAX;
      CT_Vec3f *c = NULL;
      for (size_t i = 0; i < num; i++) {
        const float d = ct_distsq2fv((CT_Vec2f *)p, (CT_Vec2f *)res[i]);
        if (d >= 1 && d < minD) {
          minD = d;
          c    = res[i];
        }
      }
      if (c) {
#ifdef CT_FEATURE_SSE4
        const __m128 cmm = c->mmval;
        p->mmval = (p->mmval - cmm) * _mm_rsqrt_ps(_mm_load1_ps(&minD)) + cmm;
#else
        const float t = 1.0f / sqrtf(minD);
        p->x          = c->x + t * (p->x - c->x);
        p->y          = c->y + t * (p->y - c->y);
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
  dla.particles = malloc(dla.length * sizeof(CT_Vec3f));
  ct_spgrid_init(&dla.accel, FVEC(0, 0), FVEC(dla.width, dla.height),
                 IVEC(120, 120), 2, 0x8000);
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
