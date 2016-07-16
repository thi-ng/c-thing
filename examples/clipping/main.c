#include "geom/clip/vatti.h"
#include "io/svg.h"

static void export_poly(CT_ClipNode *poly) {
  CT_SVGAttribs *doc_attr =
      ct_svg_attribs(0, 3, SVG_INT("width", 200), SVG_INT("height", 200),
                     SVG_HEX("stroke", rand() & 0xffffff));
  FILE *out = fopen("clip.svg", "w");
  ct_svg_start_doc(out, doc_attr);
  while (poly) {
    CT_ClipNode *aux;
    for (aux = poly; aux->next; aux = aux->next) {
      ct_svg_write_line(out, aux->pos.x, aux->pos.y, aux->next->pos.x,
                        aux->next->pos.y, NULL);
    }
    ct_svg_write_line(out, aux->pos.x, aux->pos.y, poly->pos.x, poly->pos.y,
                      NULL);
    poly = poly->nextPoly;
  }
  ct_svg_end_doc(out);
  fclose(out);
}

int main() {
  srand(time(0));
  CT_Vec2f a[]        = {{1, 1}, {100, 1}, {50, 86}};
  CT_Vec2f b[]        = {{10, 10}, {90, 10}, {90, 50}, {10, 50}};
  CT_ClipNode *s      = ct_create_polygon2f(a, 3);
  CT_ClipNode *c      = ct_create_polygon2f(b, 4);
  CT_ClipNode *result = ct_clip_polygon2f(s, c, 2);
  ct_trace_polygon2f(result);
  export_poly(result);
  ct_free_poly(result);
  ct_free_poly(s);
  ct_free_poly(c);
}
