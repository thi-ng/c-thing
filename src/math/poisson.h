#pragma once

#include <stddef.h>

#include "common/dbg.h"
#include "data/quadtree.h"
#include "geom/circle.h"
#include "math/vec.h"

typedef float (*CT_PoissonDiskGen)(CT_Quadtree *t, CT_Circle2f *);

int ct_poisson_sample2f(CT_Quadtree *t, float radius, size_t num,
                        CT_Vec2f *out);

int ct_poisson_sample2f_with(CT_Quadtree *t, CT_PoissonDiskGen gen, size_t num,
                             CT_Vec2f *out);
