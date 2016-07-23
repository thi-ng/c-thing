#pragma once

#include <stdio.h>

#include "cthing.h"

#include "data/hashtable.h"
#include "math/vec.h"

#define SVG_STR(k, v) (k), 's', (v)
#define SVG_FLOAT(k, v) (k), 'f', (v)
#define SVG_INT(k, v) (k), 'd', (v)
#define SVG_HEX(k, v) (k), 'x', (v)

CT_BEGIN_DECLS

typedef struct {
  CT_Hashtable attribs;
  size_t flags;
} CT_SVGAttribs;

CT_SVGAttribs *ct_svg_attribs(size_t flags, size_t num, ...);

int ct_svg_start_doc(FILE *out, CT_SVGAttribs *attribs);
int ct_svg_end_doc(FILE *out);

int ct_svg_start_group(FILE *out, CT_SVGAttribs *attribs);
int ct_svg_end_group(FILE *out);

int ct_svg_write_circle(FILE *out,
                        float x,
                        float y,
                        float r,
                        CT_SVGAttribs *attribs);

int ct_svg_write_rect(FILE *out,
                      float x,
                      float y,
                      float w,
                      float h,
                      CT_SVGAttribs *attribs);

int ct_svg_write_line(FILE *out,
                      float x1,
                      float y1,
                      float x2,
                      float y2,
                      CT_SVGAttribs *attribs);
int ct_svg_write_line2fv(FILE *out,
                         const CT_Vec2f *a,
                         const CT_Vec2f *b,
                         CT_SVGAttribs *attribs);
CT_END_DECLS
