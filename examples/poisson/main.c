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
} Config;

// clang-format off
static Config config = {
  .minDist = 2.0f,
  .maxDist = 10.0f,
  .dotRadius = 1.0f,
  .quality = 1e2,
  .gamma = 3.0f,
  .invert = 0,
  .bg = 0xffffff,
  .fg = 0x0000ff
};
// clang-format on

static float disc_gen(CT_Quadtree *t, CT_Circle2f *disc, void *state) {
  ct_set2fxy(&disc->pos, ct_rand_normpos() * t->root.w,
             ct_rand_normpos() * t->root.h);
  size_t x               = (size_t)disc->pos.x;
  size_t y               = (size_t)disc->pos.y;
  Config *config         = (Config *)state;
  float tt               = (float)config->image[y * config->width + x] / 255.f;
  if (config->invert) tt = 1.0f - tt;
  return disc->r =
             ct_mixf(config->minDist, config->maxDist, powf(tt, config->gamma));
}

static int poisson_svg(const Config *config) {
  CT_Quadtree t;
  CT_MPool vpool;
  ct_qtree_init(&t, 0, 0, config->width, config->height, 0x4000);
  ct_mpool_init(&vpool, 0x4000, sizeof(CT_Vec2f));
  ct_svg_write_header(stdout,
                      ct_svg_attribs(1, 2, SVG_INT("width", config->width),
                                     SVG_INT("height", config->height)));
  ct_svg_write_rect(stdout, 0, 0, config->width, config->height,
                    ct_svg_attribs(1, 1, SVG_HEX("fill", config->bg)));
  fprintf(stdout, "<g stroke=\"none\" fill=\"#%06x\">\n", config->fg);
  srand(time(0));
  size_t count  = 0;
  size_t failed = 0;
  CT_Vec2f *p   = (CT_Vec2f *)ct_mpool_alloc(&vpool);
  while (1) {
    if (!ct_poisson_sample2f_with(&t, disc_gen, (void *)config, 5, p)) {
      ct_svg_write_circle(stdout, p->x, p->y, config->dotRadius, NULL);
      p = (CT_Vec2f *)ct_mpool_alloc(&vpool);
      count++;
      failed = 0;
    } else if (++failed > config->quality) {
      break;
    }
  }
  fprintf(stdout, "</g></svg>");
  fprintf(stderr, "%zu points", count);
  ct_mpool_free(&vpool);
  ct_qtree_free(&t);
  return 0;
}

static inline uint8_t luminance(uint8_t r, uint8_t g, uint8_t b) {
  return (uint8_t)(((uint32_t)r + r + r + b + g + g + g + g) >> 3);
}

static void grayscale_image(Config *config) {
  size_t size   = config->width * config->height;
  uint8_t *gray = malloc(size);
  for (size_t i = 0, j = 0; i < size; i++, j += config->channels) {
    gray[i] =
        luminance(config->image[j], config->image[j + 1], config->image[j + 2]);
  }
  stbi_image_free(config->image);
  config->image = gray;
}

static int convert(Config *config) {
  // clang-format on
  config->image = stbi_load(config->path, &config->width, &config->height,
                            &config->channels, 0);
  if (config->image == NULL) {
    fprintf(stderr, "error loading image: %s\n", config->path);
    return 1;
  }

  fprintf(stderr, "min: %f, max: %f, r: %f, q: %u, fg: %06x, bg: %06x\n",
          config->minDist, config->maxDist, config->dotRadius, config->quality,
          config->fg, config->bg);
  fprintf(stderr, "image size: %d x %d @ %d\n", config->width, config->height,
          config->channels);

  if (config->channels > 1) {
    grayscale_image(config);
  }

  return poisson_svg(config);
}

static void usage(const Config *config) {
  fprintf(stderr, "Usage:\tex-poisson [options] image [ > out.svg ]\n");
  fprintf(stderr, "\t-b HEX\t\tbg color (default: %06x)\n", config->bg);
  fprintf(stderr, "\t-f HEX\t\tfg color (default: %06x)\n", config->fg);
  fprintf(stderr, "\t-g FLOAT\tgamma (default: %1.2f)\n", config->gamma);
  fprintf(stderr, "\t-i\t\tinvert (also swaps fg/bg) (default: %s)\n",
          config->invert ? "yes" : "no");
  fprintf(stderr, "\t-m FLOAT\tmin distance (default: %1.2f)\n",
          config->minDist);
  fprintf(stderr, "\t-x FLOAT\tmax distance (default: %1.2f)\n",
          config->maxDist);
  fprintf(stderr, "\t-r FLOAT\tdot radius (default: %1.2f)\n",
          config->dotRadius);
  fprintf(stderr, "\t-q INT\t\tquality (default: %u)\n", config->quality);
}

int main(int argc, char **argv) {
  char c;
  uint8_t err = 0;
  while (!err && (c = getopt(argc, argv, "b:f:g:im:q:r:x:")) != -1) {
    switch (c) {
      case 'm':
        config.minDist = ct_parse_float(optarg, config.minDist);
        break;
      case 'x':
        config.maxDist = ct_parse_float(optarg, config.maxDist);
        break;
      case 'r':
        config.dotRadius = ct_parse_float(optarg, config.dotRadius);
        break;
      case 'g':
        config.gamma = ct_parse_float(optarg, config.gamma);
        break;
      case 'q':
        config.quality = (uint32_t)ct_parse_float(optarg, config.quality);
        break;
      case 'f':
        config.fg = ct_parse_hex32(optarg, config.fg);
        break;
      case 'b':
        config.bg = ct_parse_hex32(optarg, config.bg);
        break;
      case 'i':
        config.invert = !config.invert;
        config.fg ^= config.bg;
        config.bg ^= config.fg;
        config.fg ^= config.bg;
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
    usage(&config);
    return 1;
  }
  config.path = argv[optind];
  return convert(&config);
}
