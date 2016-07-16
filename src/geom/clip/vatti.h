#include "cthing.h"

#include "math/vec.h"

typedef struct CT_ClipNode CT_ClipNode;

struct CT_ClipNode {
  CT_Vec2f pos;
  CT_ClipNode *next;
  CT_ClipNode *prev;
  CT_ClipNode *nextPoly;  // pointer to the next polygon
  CT_ClipNode *neighbor;  // the coresponding intersection point
  int intersect;          // 1 if an intersection point, 0 otherwise
  int entry;              // 1 if an entry point, 0 otherwise
  int visited;            // 1 if the CT_ClipNode has been visited, 0 otherwise
  float alpha;            // intersection point placement
};

CT_ClipNode *ct_create_polygon2f(CT_Vec2f *points, size_t num);
CT_ClipNode *ct_clip_polygon2f(CT_ClipNode *s, CT_ClipNode *c, int mode);
void ct_free_poly(CT_ClipNode *p);
void ct_trace_clip_node(CT_ClipNode *p);
void ct_trace_polygon2f(CT_ClipNode *p);
