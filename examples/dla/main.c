#include <stdlib.h>
#include <time.h>
#include "common/dbg.h"

#include "io/svg.h"
#include "math/math.h"
#include "math/vec.h"

typedef struct {
  CT_Vec2f *particles;
  size_t length;
  size_t num;
  size_t width;
  size_t height;
} DLA;

ct_inline CT_Vec2f *rand_particle(DLA *dla) {
  CT_Vec2f *p = &dla->particles[dla->num++];
  ct_set2fxy(p, ct_rand_normpos() * dla->width,
             ct_rand_normpos() * dla->height);
  return p;
}

static void add_particle(DLA *dla) {
  CT_Vec2f *p = rand_particle(dla);
  CT_Vec2f *c;

  float minD = 1e9;
  for (size_t i = 0, num = dla->num - 1; i < num; i++) {
    CT_Vec2f *q = &dla->particles[i];
    float d     = ct_distsq2fv(p, q);
    if (d < minD) {
      c    = q;
      minD = d;
    }
  }

  float t = 1.0f / sqrtf(minD);
  p->x    = c->x + t * (p->x - c->x);
  p->y    = c->y + t * (p->y - c->y);
}

int main() {
  DLA dla = {.width = 600, .height = 600};
  dla.length    = dla.width * dla.height >> 2;
  dla.particles = calloc(dla.length, sizeof(CT_Vec2f));
  srand(time(0));
  for (int i = 0; i < 100; i++) {
    rand_particle(&dla);
  }
  while (dla.num < dla.length) {
    add_particle(&dla);
  }
  ct_svg_write_header(stdout, ct_svg_attribs(1, 2, SVG_INT("width", dla.width),
                                             SVG_INT("height", dla.height)));
  fprintf(stdout, "<g stroke=\"none\" fill=\"#0000ff\">\n");
  for (size_t i = 0; i < dla.num; i++) {
    CT_Vec2f *p = &dla.particles[i];
    ct_svg_write_circle(stdout, p->x, p->y, 1, NULL);
  }
  fprintf(stdout, "</g></svg>");
  fprintf(stderr, "%zu particles", dla.num);
  return 0;
}
