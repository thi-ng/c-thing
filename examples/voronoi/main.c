#include "geom/voronoi.h"
#include "io/svg.h"
#include "math/poisson.h"

#define NUM_SITES 2000
#define LIMIT 1e5
#define WIDTH 600
#define DIST 10

static void write_edge(CT_VOEdge *e, void *state) {
  CT_VOVertex *a = e->ep[0], *b = e->ep[1];
  if (a && b) {
    ct_svg_write_line2fv((FILE *)state, &a->pos, &b->pos, NULL);
  }
}

int main() {
  srand(time(0));
  CT_Quadtree qt;
  CT_Voronoi vor;
  FILE *out = fopen("voronoi.svg", "w");
  ct_svg_start_doc(
      out, ct_svg_attribs(1, 3, SVG_INT("width", WIDTH),
                          SVG_INT("height", WIDTH), SVG_HEX("fill", 0xff00cc)));
  CT_Vec2f sites[NUM_SITES];
  ct_qtree_init(&qt, 0, 0, WIDTH, WIDTH, 0x1000);
  ct_set2fxy(&sites[0], -LIMIT, -LIMIT);
  ct_set2fxy(&sites[1], LIMIT, -LIMIT);
  ct_set2fxy(&sites[2], 0, LIMIT);
  size_t num = 3;
  while (num < NUM_SITES) {
    if (ct_poisson_sample2f(&qt, DIST, 100, &sites[num])) break;
    ct_svg_write_circle(out, sites[num].x, sites[num].y, 2, NULL);
    num++;
  }
  ct_svg_start_group(out, ct_svg_attribs(1, 1, SVG_HEX("stroke", 0xff)));
  ct_voronoi_init(&vor, num);
  clock_t t0 = clock();
  ct_voronoi_compute(&vor, sites, num, write_edge, out);
  double taken = (double)(clock() - t0) / CLOCKS_PER_SEC * 1000.0;
  printf("voronoi: %1.3fms (%zu sites)\n", taken, num);
  ct_voronoi_free(&vor);
  ct_qtree_free(&qt);
  ct_svg_end_group(out);
  ct_svg_end_doc(out);
  fclose(out);
  return 0;
}
