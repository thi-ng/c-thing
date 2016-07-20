#pragma once

#include "cthing.h"
#include "math/vec.h"
#include "mem/mpool.h"

#define VORONOI_DELETED -2

typedef struct {
  CT_Vec2f pos;
  uint32_t id;
  uint32_t refs;
} CT_VOVertex;

typedef struct {
  CT_VOVertex *ep[2];
  CT_VOVertex *reg[2];
  uint32_t id;
  float a, b, c;
} CT_VOEdge;

typedef struct CT_VOHalfEdge CT_VOHalfEdge;

struct CT_VOHalfEdge {
  CT_VOHalfEdge *next;
  CT_VOHalfEdge *left;
  CT_VOHalfEdge *right;
  CT_VOEdge *parent;
  CT_VOVertex *vertex;
  uint32_t refs;
  float key;
  uint8_t side;
};

typedef struct {
  CT_VOHalfEdge **hash;
  CT_VOHalfEdge *leftend;
  CT_VOHalfEdge *rightend;
  uint32_t hashSize;
  float xmin;
  float deltax;
  CT_MPool pool;
} CT_VOEdgeList;

typedef struct {
  CT_VOHalfEdge *hash;
  uint32_t hashSize;
  uint32_t count;
  uint32_t min;
  float ymin;
  float deltay;
  CT_MPool pool;
} CT_VOHeap;

typedef struct {
  CT_MPool vpool;
  CT_MPool epool;
  uint32_t numVertices;
  uint32_t numSites;
  uint32_t numEdges;
  CT_VOVertex *first;
  CT_VOEdgeList halfEdges;
  CT_VOHeap heap;
} CT_Voronoi;

typedef CT_VOVertex *(*CT_VOVertexProvider)(CT_Voronoi *, void *state);
