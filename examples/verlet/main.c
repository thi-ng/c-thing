#include "io/svg.h"
#include "math/math.h"
#include "math/vec.h"
#include "sim/verlet.h"

#define NUM 8192

static CT_SVGAttribs *attribs;

void outputFrame(CT_Verlet *phys, size_t frame) {
  char fname[64];
  float p[2];
  snprintf(fname, 64, "assets/verlet-%04zd.svg", frame);
  CT_INFO("---------- writing: %s", fname);
  FILE *out = fopen(fname, "w");
  ct_svg_write_header(out, attribs);
  fprintf(out,
          "<g stroke=\"none\" fill=\"#0000ff\" font-family=\"sans-serif\" "
          "font-size=\"9px\" text-anchor=\"middle\">\n");
  for (size_t i = 0; i < phys->num; i++) {
    ct_verlet_pos2f(phys, i, p);
    ct_svg_write_circle(out, p[0], p[1], 1.5, NULL);
    //fprintf(out, "<text x=\"%d\" y=\"%d\">%zu</text>", (int)p[0], (int)p[1], i);
  }
  fprintf(out, "</g></svg>");
  fclose(out);
}

int main() {
  // clang-format off
  CT_Verlet phys = {
    .bounds   = {10, 10, 590, 590},
    .gravity  = {0.05, 0.1},
    .friction = 0.3,
    .minD     = 6,
    .iter = 2
  };
  // clang-format off
  if (ct_verlet_init(&phys, NUM, 10, IVEC(120, 120, 120))) {
    return 1;
  }
  ct_spgrid_trace(&phys.accel);
  srand(time(0));
  for (size_t i = 0; i < NUM; i++) {
    ct_verlet_set2f(&phys, i,
                    FVEC(ct_rand_normpos() * 600, ct_rand_normpos() * 600));
  }
  attribs = ct_svg_attribs(0, 2, SVG_INT("width", 600), SVG_INT("height", 600));
  outputFrame(&phys, 0);
  for (size_t i = 1; i < 300; i++) {
    size_t num = MIN(i * 32, NUM);
    CT_Vec2f *p = (CT_Vec2f*)&phys.pos[num - 1];
    for(size_t j = phys.num; j< num; j++) {
      ct_verlet_set2f(&phys, j,
                    FVEC(ct_rand_norm() * 4 + p->x, ct_rand_norm() * 4 + p->y));
    }
    phys.num = num;
    CT_TIMED(ct_verlet_update2d(&phys));
    outputFrame(&phys, i);
  }
  ct_verlet_free(&phys);
  return 0;
}
