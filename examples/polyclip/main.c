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
  //CT_Vec2f a[] = {{10, 10}, {590, 10}, {300, 510}};
  //CT_Vec2f b[] = {{10, 300},  {250, 100}, {400, 200},
  //                {590, 100}, {350, 300}, {200, 200}};
  CT_Vec2f a[] = {{108, 106}, {185, 58},  {172, 42},  {225, 10},
                  {369, 175}, {402, 232}, {418, 276}, {358, 302},
                  {378, 256}, {247, 188}, {186, 119}, {132, 148}};
  CT_Vec2f b[] = {{78, 236},  {115, 276}, {170, 212}, {181, 207}, {215, 234},
                  {282, 264}, {313, 175}, {296, 169}, {285, 193}, {277, 189},
                  {283, 175}, {275, 172}, {284, 160}, {273, 152}, {259, 178},
                  {241, 148}, {275, 114}, {317, 157}, {329, 147}, {261, 71},
                  {180, 138}, {139, 159}, {149, 170}};
  ct_svg_start_doc(
      stdout, ct_svg_attribs(1, 3, SVG_INT("width", 600),
                             SVG_INT("height", 600), SVG_STR("fill", "none")));
  fputs(
      "<defs><pattern id=\"P1\" width=\"10\" height=\"10\" "
      "patternUnits=\"userSpaceOnUse\"><path d=\"M0,0 L10,10\" "
      "stroke=\"#00f\"/></pattern></defs>",
      stdout);
  for (size_t i = 0; i < 4; i++) {
    CT_PolyClipContext ctx;
    ct_polygon2f_clip_ctx_init(&ctx, 4);

    CT_Polygon2f s, c;
    ct_polygon2f_init(&s, a, sizeof(a) / sizeof(CT_Vec2f));
    ct_polygon2f_init(&c, b, sizeof(b) / sizeof(CT_Vec2f));
    CT_Cons *result = ct_polygon2f_clip(&ctx, &s, &c, i);
    char tx[64];
    snprintf(tx, 64, "translate(%f,%f) scale(0.75)", (i & 1) * 300.0 - 50,
             ((i >> 1) & 1) * 300.0);
    ct_svg_start_group(stdout, ct_svg_attribs(1, 1, SVG_STR("transform", tx)));
    export_poly(stdout, &s, ct_svg_attribs(1, 1, SVG_HEX("stroke", 0xff0088)));
    export_poly(stdout, &c, ct_svg_attribs(1, 1, SVG_HEX("stroke", 0x8800ff)));
    while (result) {
      export_poly(stdout, result->value,
                  ct_svg_attribs(1, 3, SVG_HEX("stroke", 0x0000ff),
                                 SVG_STR("fill", "url(#P1)"),
                                 SVG_FLOAT("stroke-width", 3.f)));
      result = result->next;
    }
    ct_svg_end_group(stdout);
    ct_polygon2f_free(&s);
    ct_polygon2f_free(&c);
    ct_polygon2f_clip_ctx_free(&ctx);
  }
  ct_svg_end_doc(stdout);
}
