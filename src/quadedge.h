#pragma once

#include <stddef.h>
#include <stdint.h>

#include "vec.h"

typedef size_t CT_QuadEdgeRef;

typedef struct {
  CT_QuadEdgeRef next[4];
  void *data[4];
  size_t mark;
} CT_QuadEdge;

typedef void (*CT_QuadEdgeVisitor)(CT_QuadEdgeRef, void *);

// Edge orientation operators

#define QE_MASK(e) ((e) & (size_t)-4)

#define ROT(e) (QE_MASK(e) + (((e) + 1) & 3))
#define SYM(e) (QE_MASK(e) + (((e) + 2) & 3))
#define TOR(e) (QE_MASK(e) + (((e) + 3) & 3))

// Vertex/face walking operators

#define ONEXT(e) ((CT_QuadEdge *)QE_MASK(e))->next[(e)&3]
#define ROTRNEXT(e) ((CT_QuadEdge *)QE_MASK(e))->next[((e) + 1) & 3]
#define SYMDNEXT(e) ((CT_QuadEdge *)QE_MASK(e))->next[((e) + 2) & 3]
#define TORLNEXT(e) ((CT_QuadEdge *)QE_MASK(e))->next[((e) + 3) & 3]

#define RNEXT(e) (TOR(ROTRNEXT(e)))
#define DNEXT(e) (SYM(SYMDNEXT(e)))
#define LNEXT(e) (ROT(TORLNEXT(e)))

#define OPREV(e) (ROT(ROTRNEXT(e)))
#define DPREV(e) (TOR(TORLNEXT(e)))
#define RPREV(e) (SYMDNEXT(e))
#define LPREV(e) (SYM(ONEXT(e)))

// Data pointers

#define ODATA(e) ((CT_QuadEdge *)QE_MASK(e))->data[(e)&3]
#define RDATA(e) ((CT_QuadEdge *)QE_MASK(e))->data[((e) + 1) & 3]
#define DDATA(e) ((CT_QuadEdge *)QE_MASK(e))->data[((e) + 2) & 3]
#define LDATA(e) ((CT_QuadEdge *)QE_MASK(e))->data[((e) + 3) & 3]

#define MARK(e) ((CT_QuadEdge *)QE_MASK(e))->mark

CT_QuadEdgeRef ct_qedge(void);

void ct_qedge_destroy(CT_QuadEdgeRef e);

void ct_qedge_splice(CT_QuadEdgeRef a, CT_QuadEdgeRef b);

void ct_qedge_iterate(CT_QuadEdgeRef a, CT_QuadEdgeVisitor visit, void *state);
