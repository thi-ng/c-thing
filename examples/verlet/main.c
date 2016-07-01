#include "io/svg.h"
#include "math/math.h"
#include "math/vec.h"
#include "sim/verlet.h"

#define NUM 2048

static CT_SVGAttribs *doc_attr;
static CT_SVGAttribs *group_attr;
static CT_SVGAttribs *spring_attr;

void outputFrame(CT_Verlet *phys, size_t frame) {
  char fname[64];
  snprintf(fname, 64, "assets/verlet-%04zu.svg", frame);
  CT_INFO("---------- writing: %s", fname);
  FILE *out = fopen(fname, "w");
  ct_svg_start_doc(out, doc_attr);
  // springs
  ct_svg_start_group(out, spring_attr);
  for (size_t i = 0; i < phys->numS; i++) {
    CT_VPSpring *s = &phys->springs[i];
    CT_Vec2f *a    = (CT_Vec2f *)&phys->pos[s->a];
    CT_Vec2f *b    = (CT_Vec2f *)&phys->pos[s->b];
    ct_svg_write_line(out, a->x, a->y, b->x, b->y, NULL);
  }
  ct_svg_end_group(out);
  // particles
  ct_svg_start_group(out, group_attr);
  for (size_t i = 0; i < phys->numP; i++) {
    float p[3];
    ct_verlet_pos2f(phys, i, p);
    ct_svg_write_circle(out, p[0], p[1], 3, NULL);
  }
  ct_svg_end_group(out);
  ct_svg_end_doc(out);
  fclose(out);
}

void spawn_particle(CT_Verlet *phys, CT_Vec2f *p, size_t id) {
  CT_Vec2f q;
  int ok;
  float r       = 5;
  float raccept = (r * 0.5) * (r * 0.5);
  do {
    q.x = ct_clampf(ct_rand_normpos() * r * 2 + p->x, 0, 600);
    q.y = ct_clampf(ct_rand_normpos() * r * 2 + p->y, 0, 600);
    ok  = (id == 0);
    for (size_t i = 0, j = id << 1; i < j && !ok; i += 2) {
      ok = (ct_distsq2fv(&q, (CT_Vec2f *)&phys->pos[i]) >= raccept);
    }
  } while (!ok);
  ct_verlet_set2f(phys, id, (float *)&q, r);
  if (id > 0) {
    ct_verlet_set_spring2d(phys, phys->numS, id - 1, id, 16, 0.5);
    phys->numS++;
  }
}

int main() {
  // clang-format off
  CT_Verlet phys = {
    .bounds    = {20, 20, 580, 580},
    .gravity   = {0.05, 0.1},
    .friction  = 0.5,
    .repulsion = 1.0,
    .maxForce  = 20,
    .iter      = 2
  };
  // clang-format off
  if (ct_verlet_init2d(&phys, NUM, NUM, IVEC(120, 120, 120))) {
    return 1;
  }
  ct_spgrid_trace(&phys.accel);
  srand(time(0));
  doc_attr = ct_svg_attribs(0, 2, SVG_INT("width", 600), SVG_INT("height", 600));
  group_attr = ct_svg_attribs(0, 2, SVG_STR("stroke", "none"), SVG_HEX("fill", 0x0000ff));
  spring_attr = ct_svg_attribs(0, 1, SVG_HEX("stroke", 0xff0000));
  outputFrame(&phys, 0);
  for (size_t i = 1; i < 300; i++) {
    size_t num = MIN(i * 8, NUM);
    CT_Vec2f *p = i > 1 ? (CT_Vec2f*)&phys.pos[num - 1] : (CT_Vec2f*)FVEC(300,300);
    for(size_t j = phys.numP; j< num; j++) {
      spawn_particle(&phys, p, j);
    }
    phys.numP = num;
    CT_TIMED(ct_verlet_update2d(&phys));
    outputFrame(&phys, i);
  }
  ct_verlet_free(&phys);
  return 0;
}
