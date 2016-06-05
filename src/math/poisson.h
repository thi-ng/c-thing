#pragma once

#include <stddef.h>

#include "common/dbg.h"
#include "data/quadtree.h"
#include "math/vec.h"

CT_Vec2f *ct_poisson_sample2f(CT_Quadtree *t, float radius, size_t num,
                              CT_Vec2f *out);
