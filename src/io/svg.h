#pragma once

#include <stddef.h>
#include <stdio.h>

#include "data/hashtable.h"

#define SVG_STR(k, v) (k), 's', (v)
#define SVG_FLOAT(k, v) (k), 'f', (v)
#define SVG_INT(k, v) (k), 'd', (v)
#define SVG_HEX(k, v) (k), 'x', (v)

typedef struct {
  CT_Hashtable attribs;
  size_t flags;
} CT_SVGAttribs;

CT_SVGAttribs *ct_svg_attribs(size_t flags, size_t num, ...);

int ct_svg_write_header(FILE *out, CT_SVGAttribs *attribs);

int ct_svg_write_circle(FILE *out, float x, float y, float r,
                        CT_SVGAttribs *attribs);

int ct_svg_write_rect(FILE *out, float x, float y, float w, float h,
                      CT_SVGAttribs *attribs);
