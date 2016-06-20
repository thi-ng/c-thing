#include "geom/clip.h"

#define CLIP_LB(pp, qq)         \
  p = (pp);                     \
  q = (qq);                     \
  if (q < 0 && fabs(p) < EPS) { \
    return 0;                   \
  }                             \
  r = q / p;                    \
  if (p < 0) {                  \
    if (r > b) {                \
      return 0;                 \
    } else if (r > a) {         \
      a = r;                    \
    }                           \
  } else if (p > 0) {           \
    if (r < a) {                \
      return 0;                 \
    } else if (r < b) {         \
      b = r;                    \
    }                           \
  }

// https://en.wikipedia.org/wiki/Liang%E2%80%93Barsky_algorithm

CT_EXPORT int ct_clip2f_liangbarsky(const CT_Vec2f *tl, const CT_Vec2f *br,
                                    const CT_Vec2f *la, const CT_Vec2f *lb,
                                    CT_Vec2f *ca, CT_Vec2f *cb) {
  const float dx = lb->x - la->x;
  const float dy = lb->y - la->y;
  float a        = 0.0;
  float b        = 1.0;
  float p, q, r;

  CLIP_LB(-dx, -(tl->x - la->x));
  CLIP_LB(dx, br->x - la->x);
  CLIP_LB(-dy, -(tl->y - la->y));
  CLIP_LB(dy, br->y - la->y);

  ca->x = a * dx + la->x;
  ca->y = a * dy + la->y;
  cb->x = b * dx + la->x;
  cb->y = b * dy + la->y;

  CT_DEBUG("ca: [%f,%f] cb: [%f,%f]", ca->x, ca->y, cb->x, cb->y);
  return 1;
}
