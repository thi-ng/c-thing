#include "geom/clip/vatti.h"
#include "io/svg.h"

static void export_poly(FILE *out, CT_ClipNode *poly, CT_SVGAttribs *attribs) {
  ct_svg_start_group(out, attribs);
  while (poly) {
    CT_ClipNode *aux = poly;
    fputs("<polygon points=\"", out);
    do {
      fprintf(out, "%1.3f,%1.3f ", aux->pos.x, aux->pos.y);
      aux = aux->next;
    } while (aux && aux != poly);
    fputs("\"/>", out);
    poly = poly->nextPoly;
  }
  ct_svg_end_group(out);
}

int main() {
  srand(time(0));
  CT_Vec2f a[] = {{10, 10}, {590, 10}, {300, 510}};
  CT_Vec2f b[] = {{10, 300},  {250, 100}, {400, 200},
                  {590, 100}, {350, 300}, {200, 200}};
  //CT_Vec2f a[] = {{100, 100}, {300, 200}, {400, 100}, {500, 300}, {50, 300}};
  //CT_Vec2f b[] = {{10, 10}, {590, 100}, {590, 130}, {10, 250}};
  FILE *out    = fopen("assets/vatti.svg", "w");
  ct_svg_start_doc(
      out, ct_svg_attribs(0, 3, SVG_INT("width", 600), SVG_INT("height", 600),
                          SVG_STR("fill", "none")));
  fprintf(out,
          "<defs><pattern id=\"P1\" width=\"10\" height=\"10\" "
          "patternUnits=\"userSpaceOnUse\"><path d=\"M0,0 L10,10\" "
          "stroke=\"#00f\"/></pattern></defs>");
  CT_ClipContext ctx;
  ct_clip_init_context(&ctx, 32);
  for (size_t i = 0; i < 4; i++) {
    CT_ClipNode *s      = ct_clip_create_polygon2f(&ctx, a, 3);
    CT_ClipNode *c      = ct_clip_create_polygon2f(&ctx, b, 6);
    CT_ClipNode *result = ct_clip_execute(&ctx, s, c, i);
    char tx[64];
    snprintf(tx, 64, "translate(%zu,%zu) scale(0.5)", (i & 1) * 300,
             ((i >> 1) & 1) * 300);
    ct_svg_start_group(out, ct_svg_attribs(1, 1, SVG_STR("transform", tx)));
    export_poly(out, s, ct_svg_attribs(1, 1, SVG_HEX("stroke", 0xff0088)));
    export_poly(out, c, ct_svg_attribs(1, 1, SVG_HEX("stroke", 0x8800ff)));
    export_poly(out, result, ct_svg_attribs(1, 3, SVG_HEX("stroke", 0x0000ff),
                                            SVG_STR("fill", "url(#P1)"),
                                            SVG_FLOAT("stroke-width", 3.f)));
    ct_svg_end_group(out);
    ct_clip_reset_context(&ctx);
  }
  ct_clip_free_context(&ctx);
  ct_svg_end_doc(out);
  fclose(out);
}
