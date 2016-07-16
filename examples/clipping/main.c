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
  CT_Vec2f b[] = {{10, 300}, {250, 100}, {590, 100}, {350, 300}};
  CT_SVGAttribs *doc_attr =
      ct_svg_attribs(0, 3, SVG_INT("width", 600), SVG_INT("height", 600),
                     SVG_STR("fill", "none"));
  FILE *out = fopen("vatti.svg", "w");
  ct_svg_start_doc(out, doc_attr);
  fprintf(out,
          "<defs><pattern id=\"P1\" width=\"10\" height=\"10\" "
          "patternUnits=\"userSpaceOnUse\"><line x1=\"0\" y1=\"0\" x2=\"10\" "
          "y2=\"10\" stroke=\"#00f\"/></pattern></defs>");
  for (size_t i = 0; i < 4; i++) {
    CT_ClipNode *s      = ct_create_polygon2f(a, 3);
    CT_ClipNode *c      = ct_create_polygon2f(b, 4);
    CT_ClipNode *result = ct_clip_polygon2f(s, c, i);
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
    ct_free_polygon2f(result);
    ct_free_polygon2f(s);
    ct_free_polygon2f(c);
  }
  ct_svg_end_doc(out);
  fclose(out);
}
