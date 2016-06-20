#include "io/svg.h"
#include "math/math.h"
#include "sim/ca.h"

int main(int argc, char** argv) {
  srand(time(0));
  const size_t states = (argc > 1) ? atol(argv[1]) : 2;
  const size_t width  = (argc > 2) ? atol(argv[2]) : 128;
  const float d       = 600.0f / (width + 1);
  const float r       = d * 0.5;
  CT_CARule2D rule = {.birthRule    = 0b1000,
                      .survivalRule = 0b1100,
                      .kernelWidth  = 1,
                      .numStates    = states,
                      .flags        = CT_CA_TILING};
  CT_CAMatrix mat = {.width = width, .height = width};
  ct_carule2d_init(&rule);
  ct_camatrix_init(&mat);
  ct_camatrix_seed_noise(&mat, 0.5f);
  char fname[64];
  snprintf(fname, 64, "assets/ca2d.svg");
  FILE* out = fopen(fname, "w");
  ct_svg_write_header(
      out, ct_svg_attribs(1, 2, SVG_INT("width", 600), SVG_INT("height", 600)));
  fprintf(out, "<g stroke=\"none\" fill=\"#0000ff\">");
  for (size_t i = 0; i < 100; i++) {
    ct_carule2d_evolve(&rule, &mat);
  }
  uint8_t* cell = mat.matrix;
  for (size_t i = 0; i < width; i++) {
    for (size_t j = 0; j < width; j++) {
      if (*cell) {
        ct_svg_write_circle(out, j * d + r, i * d + r, r, NULL);
      }
      cell++;
    }
  }
  fprintf(out, "</g></svg>");
  fclose(out);
  return 0;
}
