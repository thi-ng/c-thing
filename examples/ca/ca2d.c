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
  for (size_t gen = 0; gen < 300; gen++) {
    char fname[64];
    snprintf(fname, 64, "assets/ca2d-%04zu.svg", gen);
    CT_INFO("---------- writing: %s", fname);
    FILE* out = fopen(fname, "w");
    ct_svg_start_doc(out, ct_svg_attribs(1, 2, SVG_INT("width", 600),
                                         SVG_INT("height", 600)));
    ct_svg_start_group(out, ct_svg_attribs(1, 2, SVG_STR("stroke", "none"),
                                           SVG_HEX("fill", 0x0000ff)));
    uint8_t* cell = mat.matrix;
    for (size_t i = 0; i < width; i++) {
      for (size_t j = 0; j < width; j++) {
        if (*cell) {
          ct_svg_write_circle(out, j * d + r, i * d + r, r, NULL);
        }
        cell++;
      }
    }
    ct_svg_end_group(out);
    ct_svg_end_doc(out);
    fclose(out);
    ct_carule2d_evolve(&rule, &mat);
  }
  return 0;
}
