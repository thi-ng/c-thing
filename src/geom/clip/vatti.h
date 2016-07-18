#include "cthing.h"

#include "math/vec.h"

typedef struct CT_ClipNode CT_ClipNode;

struct CT_ClipNode {
  CT_Vec2f pos;
  CT_ClipNode *next;
  CT_ClipNode *prev;
  CT_ClipNode *nextPoly;  // pointer to the next polygon
  CT_ClipNode *neighbor;  // the coresponding intersection point
  float alpha;            // intersection point placement
  uint8_t intersect : 1;  // 1 if an intersection point, 0 otherwise
  uint8_t entry : 1;      // 1 if an entry point, 0 otherwise
  uint8_t visited : 1;    // 1 if the CT_ClipNode has been visited, 0 otherwise
};

typedef struct { CT_MPool pool; } CT_ClipContext;

int ct_clip_init_context(CT_ClipContext *ctx, size_t num);
void ct_clip_free_context(CT_ClipContext *ctx);
void ct_clip_reset_context(CT_ClipContext *ctx);

CT_ClipNode *ct_clip_create_node(CT_ClipContext *ctx, const CT_Vec2f *pos,
                                 CT_ClipNode *next, CT_ClipNode *prev,
                                 CT_ClipNode *nextPoly, CT_ClipNode *neighbor,
                                 int intersect, int entry, int visited,
                                 float alpha);
CT_ClipNode *ct_clip_create_polygon2f(CT_ClipContext *ctx,
                                      const CT_Vec2f *points, size_t num);
CT_ClipNode *ct_clip_execute(CT_ClipContext *ctx, CT_ClipNode *s,
                             CT_ClipNode *c, int mode);
void ct_clip_free_node(CT_ClipContext *ctx, CT_ClipNode *p);
void ct_clip_trace_node(const CT_ClipNode *p);
void ct_clip_trace_polygon2f(const CT_ClipNode *p);
