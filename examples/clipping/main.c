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
  CT_Vec2f a[]        = {{1, 1}, {100, 1}, {50, 86}};
  CT_Vec2f b[]        = {{10, 10}, {90, 10}, {90, 50}, {10, 50}};
  CT_ClipNode *s      = ct_create_polygon2f(a, 3);
  CT_ClipNode *c      = ct_create_polygon2f(b, 4);
  CT_ClipNode *result = ct_clip_polygon2f(s, c, 0);
  ct_trace_polygon2f(result);
  CT_SVGAttribs *doc_attr =
      ct_svg_attribs(0, 3, SVG_INT("width", 200), SVG_INT("height", 200),
                     SVG_STR("fill", "none"));
  FILE *out = fopen("clip.svg", "w");
  ct_svg_start_doc(out, doc_attr);
  export_poly(out, s, ct_svg_attribs(1, 1, SVG_HEX("stroke", 0xff0000)));
  export_poly(out, c, ct_svg_attribs(1, 1, SVG_HEX("stroke", 0xff00)));
  export_poly(out, result, ct_svg_attribs(1, 2, SVG_HEX("stroke", 0xff),
                                          SVG_FLOAT("stroke-width", 3.f)));
  ct_svg_end_doc(out);
  fclose(out);
  ct_free_poly(result);
  ct_free_poly(s);
  ct_free_poly(c);
}
