#include <unistd.h>

#include "geom/voronoi.h"
#include "io/svg.h"
#include "math/math.h"
#include "math/poisson.h"

#define NUM_SITES 20000
#define LIMIT 1e4
#define WIDTH 600
#define DIST 10
#define QUALITY 100

typedef struct {
  CT_Vec2f *sites;
  int width;
  int height;
  int num;
  int quality;
  int showSites;
  int pipe;
  float dist;
  float limit;
} AppState;

// clang-format off
static AppState state = {
  .width     = WIDTH,
  .height    = WIDTH,
  .num       = NUM_SITES,
  .dist      = DIST,
  .quality   = QUALITY,
  .limit     = LIMIT,
  .pipe      = 0,
  .showSites = 0
};
// clang-format on

static void write_edge(CT_VOEdge *e, void *_) {
  CT_VOVertex *a = e->ep[0], *b = e->ep[1];
  if (a && b) {
    ct_svg_write_line2fv(stdout, &a->pos, &b->pos, NULL);
  }
}

static void voronoi_poisson(AppState *state) {
  srand(time(0));
  CT_Quadtree qt;
  CT_Voronoi vor;
  size_t num = 3;
  ct_qtree_init(&qt, 0, 0, state->width, state->height, 0x1000);
  while (num < state->num) {
    if (ct_poisson_sample2f(&qt, state->dist, state->quality,
                            &state->sites[num]))
      break;
    if (state->showSites) {
      ct_svg_write_circle(stdout, state->sites[num].x, state->sites[num].y, 2,
                          NULL);
    }
    num++;
  }
  ct_svg_start_group(stdout, ct_svg_attribs(1, 1, SVG_HEX("stroke", 0xff)));
  ct_voronoi_init(&vor, num);
  clock_t t0 = clock();
  ct_voronoi_compute(&vor, state->sites, num, write_edge, stdout);
  double taken = (double)(clock() - t0) / CLOCKS_PER_SEC * 1000.0;
  fprintf(stderr, "voronoi: %1.3fms (%zu sites)\n", taken, num);
  ct_voronoi_free(&vor);
  ct_qtree_free(&qt);
}

static void voronoi_pipe(AppState *state) {
  size_t num = 3;
  while (num < state->num &&
         fscanf(stdin, "%f,%f", &state->sites[num].x, &state->sites[num].y) !=
             EOF) {
    if (state->showSites) {
      ct_svg_write_circle(stdout, state->sites[num].x, state->sites[num].y, 2,
                          NULL);
    }
    num++;
  }
  ct_svg_start_group(stdout, ct_svg_attribs(1, 1, SVG_HEX("stroke", 0xff)));
  CT_Voronoi vor;
  ct_voronoi_init(&vor, num);
  clock_t t0 = clock();
  ct_voronoi_compute(&vor, state->sites, num, write_edge, stdout);
  double taken = (double)(clock() - t0) / CLOCKS_PER_SEC * 1000.0;
  fprintf(stderr, "voronoi: %1.3fms (%zu sites)\n", taken, num);
  ct_voronoi_free(&vor);
}

void usage(AppState *state) {
  fprintf(stderr, "TODO usage:\n");
}

int main(int argc, char **argv) {
  char c;
  uint8_t err = 0;
  while (!err && (c = getopt(argc, argv, "d:h:l:n:pq:sw:")) != -1) {
    switch (c) {
      case 'd':
        state.dist = ct_parse_float(optarg, state.dist);
        break;
      case 'l':
        state.limit = ct_parse_float(optarg, state.limit);
        break;
      case 'q':
        state.quality = ct_parse_int(optarg, state.quality);
        break;
      case 's':
        state.showSites = 1;
        break;
      case 'w':
        state.width = ct_parse_int(optarg, state.width);
        break;
      case 'h':
        state.height = ct_parse_int(optarg, state.height);
        break;
      case 'n':
        state.num = ct_parse_int(optarg, state.num);
        break;
      case 'p':
        state.pipe = 1;
        break;
      case '?':
        fprintf(stderr, "Option -%c requires an argument\n", optopt);
        err = 1;
        break;
      default:
        err = 1;
    }
  }
  if (err) {
    usage(&state);
    return 1;
  }
  state.sites = malloc(state.num * sizeof(CT_Vec2f));
  // define surrounding triangle
  ct_set2fxy(&state.sites[0], -state.limit, -state.limit);
  ct_set2fxy(&state.sites[1], state.limit, -state.limit);
  ct_set2fxy(&state.sites[2], state.width / 2, state.limit);

  ct_svg_start_doc(stdout, ct_svg_attribs(1, 3, SVG_INT("width", state.width),
                                          SVG_INT("height", state.height),
                                          SVG_HEX("fill", 0xff00cc)));
  if (state.pipe) {
    voronoi_pipe(&state);
  } else {
    voronoi_poisson(&state);
  }
  ct_svg_end_group(stdout);
  ct_svg_end_doc(stdout);
  return 0;
}
