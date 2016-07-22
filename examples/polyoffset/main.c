#include "geom/polygon.h"
#include "io/svg.h"

static void export_poly(FILE *out, CT_Polygon2f *poly, CT_SVGAttribs *attribs) {
  ct_svg_start_group(out, attribs);
  fputs("<polygon points=\"", out);
  CT_PolyVertex *v = poly->first;
  do {
    fprintf(out, "%1.3f,%1.3f ", v->pos.x, v->pos.y);
    v = v->next;
  } while (v != poly->first);
  fputs("\"/>", out);
  ct_svg_end_group(out);
}

int main() {
  CT_Vec2f a[] = {{-157, -53}, {-80, -101}, {-93, -117}, {-40, -149},
                  {104, 15},   {137, 72},   {153, 116},  {93, 142},
                  {113, 96},   {-18, 28},   {-79, -40},  {-133, -11}};

  CT_Vec2f b[] = {{-160, 57}, {-123, 97}, {-68, 33}, {-57, 28}, {-23, 55},
                  {43, 85},   {74, -3},   {57, -9},  {46, 14},  {38, 10},
                  {44, -3},   {36, -6},   {45, -18}, {34, -26}, {20, 0},
                  {2, -30},   {36, -64},  {78, -21}, {90, -31}, {22, -107},
                  {-58, -40}, {-99, -19}, {-89, -8}};

  ct_svg_start_doc(
      stdout,
      ct_svg_attribs(1, 4, SVG_INT("width", 600), SVG_INT("height", 600),
                     SVG_STR("fill", "none"), SVG_FLOAT("stroke-width", 0.5f)));
  ct_svg_start_group(
      stdout, ct_svg_attribs(
                  1, 1, SVG_STR("transform", "matrix(1.8 0 0 1.8 360 300)")));

  CT_Polygon2f *result;
  CT_PolyOffsetContext ctx;
  ct_polygon2f_offset_ctx_init(&ctx, 64);

  for (int i = 9; i > -10; i--) {
    result = ct_polygon2f_offset_points(&ctx, b, sizeof(b) / sizeof(CT_Vec2f),
                                        i * 2.5, 7);
    if (result) {
      uint32_t col  = abs(i) * 25;
      uint32_t col2 = col >> 1;
      col           = (col2 << 16) | (col2 << 8) | col;
      export_poly(stdout, result,
                  ct_svg_attribs(1, 1, SVG_HEX("stroke", i ? col : 0xff00cc)));
    }
    ct_polygon2f_offset_ctx_reset(&ctx);
  }

  ct_polygon2f_offset_ctx_free(&ctx);

  ct_svg_end_group(stdout);
  ct_svg_end_doc(stdout);
}
