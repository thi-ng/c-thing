#include "cthing.h"

#include "data/cons.h"
#include "math/vec.h"
#include "mem/mpool.h"

enum { CT_CLIP_UNION = 0, CT_CLIP_INTERSECTION = 1, CT_CLIP_DIFF = 2 };

typedef struct CT_PolyVertex CT_PolyVertex;

struct CT_PolyVertex {
  CT_Vec2f pos;
  CT_PolyVertex *next;
  CT_PolyVertex *prev;
  CT_PolyVertex *pair;
  float dist;
  uint8_t isec : 1;
  uint8_t entry : 1;
  uint8_t visited : 1;
};

typedef struct {
  CT_PolyVertex *first;
  CT_PolyVertex *firstIsec;
  CT_PolyVertex *lastProc;
  size_t num;
  CT_MPool pool;
} CT_Polygon2f;

typedef struct {
  CT_Vec2f curr;
  CT_Vec2f next;
  CT_Vec2f normal;
} CT_PolyEdge;

typedef struct {
  CT_MPool ppool;
  CT_MPool cpool;
  CT_Cons *result;
} CT_PolyClipContext;

typedef struct {
  CT_PolyEdge *edges;
  size_t num;
  int8_t res;
  CT_MPool vpool;
  CT_MPool cpool;
  CT_PolyClipContext clip;
} CT_PolyOffsetContext;

int ct_polygon2f_init(CT_Polygon2f *poly, CT_Vec2f *points, size_t num);
void ct_polygon2f_free(CT_Polygon2f *poly);
CT_Polygon2f *ct_polygon2f_add_vertex(CT_Polygon2f *poly, const CT_Vec2f *pos);
int ct_polygon2f_point_inside(const CT_Polygon2f *poly, const CT_Vec2f *v);
CT_Cons *ct_polygon2f_clip(CT_PolyClipContext *ctx,
                           CT_Polygon2f *src,
                           CT_Polygon2f *clip,
                           size_t mode);

int ct_polygon2f_clip_ctx_init(CT_PolyClipContext *ctx, size_t poolSize);
void ct_polygon2f_clip_ctx_free(CT_PolyClipContext *ctx);
void ct_polygon2f_clip_ctx_reset(CT_PolyClipContext *ctx);

int ct_polygon2f_offset_ctx_init(CT_PolyOffsetContext *ctx, size_t poolSize);
void ct_polygon2f_offset_ctx_free(CT_PolyOffsetContext *ctx);
void ct_polygon2f_offset_ctx_reset(CT_PolyOffsetContext *ctx);

CT_Polygon2f *ct_polygon2f_offset_points(CT_PolyOffsetContext *ctx,
                                         const CT_Vec2f *points,
                                         size_t num,
                                         float dist,
                                         int8_t res);
