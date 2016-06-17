#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <time.h>
#include "common/dbg.h"
#include "config.h"

#include "io/svg.h"
#include "math/math.h"
#include "math/vec.h"
#include "math/verlet.h"

#define NUM 640

static CT_SVGAttribs *attribs;

void outputFrame(CT_Verlet2f *phys, size_t frame) {
  char fname[64];
  float p[2];
  snprintf(fname, 64, "assets/verlet-%04zd.svg", frame);
  CT_INFO("writing: %s", fname);
  ct_verlet_trace(phys);
  FILE *out = fopen(fname, "w");
  ct_svg_write_header(out, attribs);
  fprintf(out,
          "<g stroke=\"none\" fill=\"#0000ff\" font-family=\"sans-serif\" "
          "font-size=\"9px\">\n");
  for (size_t i = 0; i < phys->num; i++) {
    ct_verlet_pos2f(phys, i, p);
    ct_svg_write_circle(out, p[0], p[1], 5, NULL);
    //fprintf(out, "<text x=\"%d\" y=\"%d\">%zu</text>", (int)p[0], (int)p[1], i);
  }
  fprintf(out, "</g></svg>");
  fclose(out);
}

int main() {
  CT_Verlet2f phys;
  if (ct_verlet_init(&phys, NUM, 1, 0.95, 20, FVEC(0, 0.2),
                     FVEC(10, 10, 590, 590))) {
    return 1;
  }
  for (size_t i = 0; i < NUM; i++) {
    ct_verlet_set2f(&phys, i,
                    FVEC(ct_rand_normpos() * 600, ct_rand_normpos() * 600), 1);
  }
  attribs = ct_svg_attribs(0, 2, SVG_INT("width", 600), SVG_INT("height", 600));
  outputFrame(&phys, 0);
  for (size_t i = 1; i < 200; i++) {
    clock_t begin = clock();
    ct_verlet_update2d(&phys);
    double measured = (double)(clock() - begin) / CLOCKS_PER_SEC * 1000.0;
    //CT_INFO("%p pos: %f,%f prev: %f,%f force: %f,%f", phys.force, phys.pos[0], phys.pos[NUM], phys.prev[0], phys.prev[NUM], phys.force[0], phys.force[NUM]);
    CT_INFO("time: %f", measured);
    outputFrame(&phys, i);
  }
  return 0;
}
