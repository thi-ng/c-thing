#include "io/svg.h"
#include "math/math.h"
#include "sim/ca.h"

int main(int argc, char** argv) {
  srand(time(0));
  const size_t rule_id = (argc > 1) ? atol(argv[1]) : rand();
  const size_t states  = (argc > 2) ? atol(argv[2]) : 2;
  const size_t kw      = (argc > 3) ? atol(argv[3]) : 1;
  const size_t width   = (argc > 4) ? atol(argv[4]) : 128;
  const float d        = 600.0f / width;
  const float r        = d * 0.5;
  CT_CARule1D rule = {.numStates   = states,
                      .kernelWidth = kw,
                      .rule        = rule_id,
                      .flags       = CT_CA_TILING};
  CT_CAMatrix mat = {.width = width, .height = 1};
  ct_carule1d_init(&rule);
  ct_camatrix_init(&mat);
  ct_camatrix_seed_noise(&mat, 0.5f);
  char fname[64];
  snprintf(fname, 64, "assets/ca-%zu.svg", rule_id);
  FILE* out = fopen(fname, "w");
  ct_svg_start_doc(
      out, ct_svg_attribs(1, 2, SVG_INT("width", 600), SVG_INT("height", 600)));
  ct_svg_start_group(out, ct_svg_attribs(1, 2, SVG_STR("stroke", "none"),
                                         SVG_HEX("fill", 0x0000ff)));
  for (size_t i = 0; i < width; i++) {
    for (size_t j = 0; j < width; j++) {
      if (mat.matrix[j]) {
        ct_svg_write_circle(out, j * d + r, i * d + r, r, NULL);
      }
    }
    ct_carule1d_evolve(&rule, &mat);
  }
  ct_svg_end_group(out);
  ct_svg_end_doc(out);
  return 0;
}
