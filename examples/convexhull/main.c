#include "io/svg.h"
#include "math/vec.h"

#define NUM 10000
#define WIDTH 600
#define HEIGHT 600
#define RADIUS (0.48 * WIDTH)
#define THETA (0.5 * CT_PI)

static void export_poly(FILE *out,
                        CT_Vec2f *poly,
                        size_t num,
                        CT_SVGAttribs *attribs) {
  ct_svg_start_group(out, attribs);
  fputs("<polygon points=\"", out);
  for (size_t i = 0; i < num; i++) {
    fprintf(out, "%1.3f,%1.3f ", poly[i].x, poly[i].y);
  }
  fputs("\"/>", out);
  ct_svg_end_group(out);
}

int main(int argc, char **argv) {
  CT_Vec2f hull[64];
  CT_Vec2f *points = calloc(NUM, sizeof(CT_Vec2f));

  ct_svg_start_doc(stdout, ct_svg_attribs(1, 3, SVG_INT("width", WIDTH),
                                          SVG_INT("height", HEIGHT),
                                          SVG_STR("fill", "none")));

  ct_svg_start_group(stdout, ct_svg_attribs(1, 1, SVG_HEX("fill", 0xff00cc)));
  for (size_t i = 0; i < NUM; i++) {
    CT_Vec2f *v = &points[i];
    float r     = ct_rand_normpos() * RADIUS;
    float t     = ct_rand_normpos() * THETA;
    t           = ct_rand_normpos() < 0.5 ? t : CT_PI + t;
    ct_add2fxy_imm(ct_cartesian2f_imm(ct_set2fxy(v, r, t)), WIDTH / 2,
                   HEIGHT / 2);
    ct_svg_write_circle(stdout, v->x, v->y, 1.5, NULL);
  }
  ct_svg_end_group(stdout);

  size_t len = ct_convexhull2f(points, NUM, hull);
  CT_INFO("hull points: %zu", len);
  export_poly(stdout, hull, len, ct_svg_attribs(1, 1, SVG_HEX("stroke", 0xff)));

  ct_svg_end_doc(stdout);

  free(points);

  return 0;
}
