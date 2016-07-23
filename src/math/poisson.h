#pragma once

#include "cthing.h"

#include "data/quadtree.h"
#include "geom/circle.h"
#include "math/vec.h"

CT_BEGIN_DECLS

typedef float (*CT_PoissonDiskGen)(CT_Quadtree *t,
                                   CT_Circle2f *disc,
                                   void *state);

int ct_poisson_sample2f(CT_Quadtree *t,
                        float radius,
                        size_t num,
                        CT_Vec2f *out);

int ct_poisson_sample2f_with(CT_Quadtree *t,
                             CT_PoissonDiskGen gen,
                             void *state,
                             size_t num,
                             CT_Vec2f *out);

CT_END_DECLS
