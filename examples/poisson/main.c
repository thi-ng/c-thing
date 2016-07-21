#include <string.h>
#include <unistd.h>

#include "io/svg.h"
#include "math/math.h"
#include "math/poisson.h"

#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_DEFINE
#include "stb.h"

typedef struct {
  CT_Quadtree tree;
  CT_MPool vpool;
  char *path;
  float minDist;
  float maxDist;
  float dotRadius;
  float gamma;
  uint32_t quality;
  uint32_t fg;
  uint32_t bg;
  uint32_t invert;
  uint8_t *image;
  int width;
  int height;
  int channels;
  int textMode;
} AppState;

// clang-format off
static AppState state = {
  .minDist = 2.0f,
  .maxDist = 10.0f,
  .dotRadius = 1.0f,
  .quality = 1e2,
  .gamma = 3.0f,
  .invert = 0,
  .bg = 0xffffff,
  .fg = 0x0000ff,
  .textMode = 0
};
// clang-format on

static float disc_gen(CT_Quadtree *t, CT_Circle2f *disc, void *s) {
  ct_set2fxy(&disc->pos, ct_rand_normpos() * t->root.w,
             ct_rand_normpos() * t->root.h);
  size_t x        = (size_t)disc->pos.x;
  size_t y        = (size_t)disc->pos.y;
  AppState *state = (AppState *)s;
  float tt        = (float)state->image[y * state->width + x] / 255.f;
  if (state->invert) {
    tt = 1.0f - tt;
  }
  return disc->r =
             ct_mixf(state->minDist, state->maxDist, powf(tt, state->gamma));
}

static int poisson_svg(AppState *state) {
  ct_svg_start_doc(stdout, ct_svg_attribs(1, 2, SVG_INT("width", state->width),
                                          SVG_INT("height", state->height)));
  ct_svg_write_rect(stdout, 0, 0, state->width, state->height,
                    ct_svg_attribs(1, 1, SVG_HEX("fill", state->bg)));
  ct_svg_start_group(stdout, ct_svg_attribs(1, 2, SVG_STR("stroke", "none"),
                                            SVG_HEX("fill", state->fg)));
  size_t count  = 0;
  size_t failed = 0;
  CT_Vec2f *p   = (CT_Vec2f *)ct_mpool_alloc(&state->vpool);
  while (1) {
    if (!ct_poisson_sample2f_with(&state->tree, disc_gen, (void *)state, 5,
                                  p)) {
      ct_svg_write_circle(stdout, p->x, p->y, state->dotRadius, NULL);
      p = (CT_Vec2f *)ct_mpool_alloc(&state->vpool);
      count++;
      failed = 0;
    } else if (++failed > state->quality) {
      break;
    }
  }
  ct_svg_end_group(stdout);
  ct_svg_end_doc(stdout);
  fprintf(stderr, "%zu points", count);
  return 0;
}

static int poisson_txt(AppState *state) {
  size_t count  = 0;
  size_t failed = 0;
  CT_Vec2f *p   = (CT_Vec2f *)ct_mpool_alloc(&state->vpool);
  while (1) {
    if (!ct_poisson_sample2f_with(&state->tree, disc_gen, (void *)state, 5,
                                  p)) {
      fprintf(stdout, "%1.3f,%1.3f\n", p->x, p->y);
      p = (CT_Vec2f *)ct_mpool_alloc(&state->vpool);
      count++;
      failed = 0;
    } else if (++failed > state->quality) {
      break;
    }
  }
  fprintf(stderr, "%zu points", count);
  return 0;
}

static inline uint8_t luminance(uint8_t r, uint8_t g, uint8_t b) {
  return (uint8_t)(((uint32_t)r + r + r + b + g + g + g + g) >> 3);
}

static void grayscale_image(AppState *state) {
  size_t size   = state->width * state->height;
  uint8_t *gray = malloc(size);
  for (size_t i = 0, j = 0; i < size; i++, j += state->channels) {
    gray[i] =
        luminance(state->image[j], state->image[j + 1], state->image[j + 2]);
  }
  stbi_image_free(state->image);
  state->image = gray;
}

static int convert(AppState *state) {
  // clang-format on
  state->image = stbi_load(state->path, &state->width, &state->height,
                           &state->channels, 0);
  if (state->image == NULL) {
    fprintf(stderr, "error loading image: %s\n", state->path);
    return 1;
  }

  fprintf(stderr, "min: %f, max: %f, r: %f, q: %u, fg: %06x, bg: %06x\n",
          state->minDist, state->maxDist, state->dotRadius, state->quality,
          state->fg, state->bg);
  fprintf(stderr, "image size: %d x %d @ %d\n", state->width, state->height,
          state->channels);

  if (state->channels > 1) {
    grayscale_image(state);
  }

  srand(time(0));
  ct_qtree_init(&state->tree, 0, 0, state->width, state->height, 0x4000);
  ct_mpool_init(&state->vpool, 0x4000, sizeof(CT_Vec2f));
  int res = (state->textMode ? poisson_txt(state) : poisson_svg(state));
  ct_mpool_free(&state->vpool);
  ct_qtree_free(&state->tree);
  return res;
}

static void usage(const AppState *state) {
  fprintf(stderr, "Usage:\tex-poisson [options] image [ > out.svg ]\n");
  fprintf(stderr, "\t-b HEX\t\tbg color (default: %06x)\n", state->bg);
  fprintf(stderr, "\t-f HEX\t\tfg color (default: %06x)\n", state->fg);
  fprintf(stderr, "\t-g FLOAT\tgamma (default: %1.2f)\n", state->gamma);
  fprintf(stderr, "\t-i\t\tinvert (also swaps fg/bg) (default: %s)\n",
          state->invert ? "yes" : "no");
  fprintf(stderr, "\t-m FLOAT\tmin distance (default: %1.2f)\n",
          state->minDist);
  fprintf(stderr, "\t-x FLOAT\tmax distance (default: %1.2f)\n",
          state->maxDist);
  fprintf(stderr, "\t-r FLOAT\tdot radius (default: %1.2f)\n",
          state->dotRadius);
  fprintf(stderr, "\t-q INT\t\tquality (default: %u)\n", state->quality);
  fprintf(stderr, "\t-t\t\toutput points as text only (default: no)\n");
}

int main(int argc, char **argv) {
  char c;
  uint8_t err = 0;
  while (!err && (c = getopt(argc, argv, "b:f:g:im:q:r:tx:")) != -1) {
    switch (c) {
      case 'm':
        state.minDist = ct_parse_float(optarg, state.minDist);
        break;
      case 'x':
        state.maxDist = ct_parse_float(optarg, state.maxDist);
        break;
      case 'r':
        state.dotRadius = ct_parse_float(optarg, state.dotRadius);
        break;
      case 'g':
        state.gamma = ct_parse_float(optarg, state.gamma);
        break;
      case 'q':
        state.quality = (uint32_t)ct_parse_float(optarg, state.quality);
        break;
      case 'f':
        state.fg = ct_parse_hex32(optarg, state.fg);
        break;
      case 'b':
        state.bg = ct_parse_hex32(optarg, state.bg);
        break;
      case 'i':
        state.invert = !state.invert;
        state.fg ^= state.bg;
        state.bg ^= state.fg;
        state.fg ^= state.bg;
        break;
      case 't':
        state.textMode = 1;
        break;
      case '?':
        fprintf(stderr, "Option -%c requires an argument\n", optopt);
        err = 1;
        break;
      default:
        err = 1;
    }
  }
  if (!err && optind >= argc) {
    fprintf(stderr, "Missing input file\n");
    err = 1;
  }
  if (!err && optind < argc - 1) {
    fprintf(stderr, "Extraneous arguments given\n");
    err = 1;
  }
  if (err) {
    usage(&state);
    return 1;
  }
  state.path = argv[optind];
  return convert(&state);
}
