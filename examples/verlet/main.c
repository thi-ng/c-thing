#include "io/svg.h"
#include "math/math.h"
#include "math/vec.h"
#include "sim/verlet.h"

#define NUM 8192

static CT_SVGAttribs *doc_attribs;
static CT_SVGAttribs *group_attribs;

void outputFrame(CT_Verlet *phys, size_t frame) {
  char fname[64];
  snprintf(fname, 64, "assets/verlet-%04zu.svg", frame);
  CT_INFO("---------- writing: %s", fname);
  FILE *out = fopen(fname, "w");
  ct_svg_start_doc(out, doc_attribs);
  ct_svg_start_group(out, group_attribs);
  for (size_t i = 0; i < phys->num; i++) {
    float p[3];
    ct_verlet_pos2f(phys, i, p);
    ct_svg_write_circle(out, p[0], p[1], p[2] * 0.75, NULL);
  }
  ct_svg_end_group(out);
  ct_svg_end_doc(out);
  fclose(out);
}

void spawn_particle(CT_Verlet *phys, CT_Vec2f *p, size_t id) {
  CT_Vec2f q;
  int ok;
  float r =
      ct_rand_normpos() < 0.1 ? ct_rand_minmax(4, 10) : ct_rand_minmax(2, 4);
  float raccept = (r * 0.5) * (r * 0.5);
  do {
    q.x = ct_rand_normpos() * r * 2 + p->x;
    q.y = ct_rand_normpos() * r * 2 + p->y;
    ok  = (id == 0);
    for (size_t i = 0, j = id << 1; i < j && !ok; i += 2) {
      ok = (ct_distsq2fv(&q, (CT_Vec2f *)&phys->pos[i]) >= raccept);
    }
  } while (!ok);
  ct_verlet_set2f(phys, id, (float *)&q, r);
}

int main() {
  // clang-format off
  CT_Verlet phys = {
    .bounds    = {20, 20, 580, 580},
    .gravity   = {0.05, 0.1},
    .friction  = 0.5,
    .repulsion = 1.0,
    .maxForce  = 20,
    .iter      = 4
  };
  // clang-format off
  if (ct_verlet_init(&phys, NUM, IVEC(120, 120, 120))) {
    return 1;
  }
  ct_spgrid_trace(&phys.accel);
  srand(time(0));
  doc_attribs = ct_svg_attribs(0, 2, SVG_INT("width", 600), SVG_INT("height", 600));
  group_attribs = ct_svg_attribs(0, 2, SVG_STR("stroke", "none"), SVG_HEX("fill", 0x0000ff));
  outputFrame(&phys, 0);
  for (size_t i = 1; i < 300; i++) {
    size_t num = MIN(i * 32, NUM);
    CT_Vec2f *p = (CT_Vec2f*)&phys.pos[num - 1];
    for(size_t j = phys.num; j< num; j++) {
      spawn_particle(&phys, p, j);
    }
    phys.num = num;
    CT_TIMED(ct_verlet_update2d(&phys));
    outputFrame(&phys, i);
  }
  ct_verlet_free(&phys);
  return 0;
}
