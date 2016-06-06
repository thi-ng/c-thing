#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "common/dbg.h"
#include "io/svg.h"
#include "math/hashfn.h"

CT_SVGAttribs *ct_svg_attribs(size_t flags, size_t num, ...) {
  CT_SVGAttribs *a = malloc(sizeof(CT_SVGAttribs));
  CT_HTOps ops = {.hash = ct_murmur3_32};
  size_t inited = 0;
  if (!ct_ht_init(&a->attribs, &ops, 4, 4, CT_HT_NONE)) {
    inited   = 1;
    a->flags = flags;
    va_list args;
    va_start(args, num);
    for (size_t i = 0; i < num; i++) {
      char *key = va_arg(args, char *);
      char type = (char)va_arg(args, int);
      size_t ks = strlen(key) + 1;
      CT_INFO("attrib: %s", key);
      switch (type) {
        case 's': {
          char *val = va_arg(args, char *);
          CT_INFO("val: %s", val);
          ct_ht_assoc(&a->attribs, key, val, ks, strlen(val) + 1);
          break;
        }
        case 'f': {
          float val = (float)va_arg(args, double);
          CT_INFO("val: %f", val);
          ct_ht_assoc(&a->attribs, key, &val, ks, sizeof(float));
          break;
        }
        case 'd': {
          int val = va_arg(args, int);
          CT_INFO("val: %d", val);
          ct_ht_assoc(&a->attribs, key, &val, ks, sizeof(int));
          break;
        }
        case 'x': {
          uint32_t val = va_arg(args, uint32_t);
          char buf[8];
          snprintf(buf, 8, "#%06x", val);
          CT_INFO("val: %s", buf);
          ct_ht_assoc(&a->attribs, key, &buf, ks, 8);
          break;
        }
        default:
          CT_SENTINEL("unsupported attrib type: %c", type);
      }
      va_end(args);
    }
    return a;
  }
fail:
  CT_ERROR("can't create SVG attribs");
  if (a != NULL) {
    if (inited) {
      ct_ht_free(&a->attribs);
    }
    free(a);
  }
  return NULL;
}

static int write_attrib(CT_HTEntry *e, void *state) {
  FILE *out = (FILE *)state;
  if (!strcmp(e->key, "width")) {
    fprintf(out, "width=\"%zu\" ", *((size_t *)e->val));
  } else if (!strcmp(e->key, "height")) {
    fprintf(out, "height=\"%zu\" ", *((size_t *)e->val));
  } else if (!strcmp(e->key, "fill")) {
    fprintf(out, "fill=\"%s\" ", (char *)e->val);
  } else if (!strcmp(e->key, "stroke")) {
    fprintf(out, "stroke=\"%s\" ", (char *)e->val);
  }
  return 0;
}

static void write_attribs(FILE *out, CT_SVGAttribs *a, char *suffix) {
  if (a) {
    ct_ht_iterate(&a->attribs, write_attrib, out);
    if (a->flags) {
      CT_INFO("free attribs: %p", a);
      ct_ht_free(&a->attribs);
      free(a);
    }
  }
  fputs(suffix, out);
}

int ct_svg_write_header(FILE *out, CT_SVGAttribs *attribs) {
  fprintf(out,
          "<?xml version=\"1.0\"?>\n<svg version=\"1.1\" "
          "xmlns:xlink=\"http://www.w3.org/1999/xlink\" "
          "xmlns=\"http://www.w3.org/2000/svg\" ");
  write_attribs(out, attribs, ">");
  return 0;
}

int ct_svg_write_circle(FILE *out, float x, float y, float r,
                        CT_SVGAttribs *attribs) {
  fprintf(out, "<circle cx=\"%f\" cy=\"%f\" r=\"%f\" ", x, y, r);
  write_attribs(out, attribs, "/>");
  return 0;
}

int ct_svg_write_rect(FILE *out, float x, float y, float w, float h,
                      CT_SVGAttribs *attribs) {
  fprintf(out, "<rect x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" ", x, y, w,
          h);
  write_attribs(out, attribs, "/>");
  return 0;
}