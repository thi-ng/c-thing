#include "geom/triangle.h"

ct_export CT_Triangle2f *ct_triangle2f_init(CT_Triangle2f *t,
                                            const CT_Vec2f *a,
                                            const CT_Vec2f *b,
                                            const CT_Vec2f *c) {
  t->a = *a;
  t->b = *b;
  t->c = *c;
  return t;
}

ct_export CT_Triangle2f *ct_triangle2f_initpv(CT_Triangle2f *t,
                                              const CT_Vec2f *points) {
  t->a = *points++;
  t->b = *points++;
  t->c = *points;
  return t;
}

ct_export CT_Triangle2f *ct_triangle2f_initpf(CT_Triangle2f *t,
                                              const float *coords) {
  t->buf[0] = *coords++;
  t->buf[1] = *coords++;
  t->buf[2] = *coords++;
  t->buf[3] = *coords++;
  t->buf[4] = *coords++;
  t->buf[5] = *coords;
  return t;
}

ct_export float ct_triangle2f_normsign(const CT_Triangle2f *t) {
  return (t->b.x - t->a.x) * (t->c.y - t->a.y) -
         (t->c.x - t->a.x) * (t->b.y - t->a.y);
}

ct_export float ct_triangle2f_area(const CT_Triangle2f *t) {
  return 0.5f * ct_triangle2f_normsign(t);
}

ct_export float ct_triangle2f_circumference(const CT_Triangle2f *t) {
  return ct_dist2fv(&t->a, &t->b) + ct_dist2fv(&t->b, &t->c) +
         ct_dist2fv(&t->c, &t->a);
}

ct_export size_t ct_triangle2f_is_clockwise(const CT_Triangle2f *t) {
  return ct_triangle2f_normsign(t) < 0.f;
}

ct_export CT_Vec2f *ct_triangle2f_point_opposite_ptr(CT_Triangle2f *t,
                                                     const CT_Vec2f *a,
                                                     const CT_Vec2f *b) {
  if (a == &t->a) {
    return (b == &t->b ? &t->c : (b == &t->c ? &t->b : NULL));
  } else if (a == &t->b) {
    return (b == &t->a ? &t->c : (b == &t->c ? &t->a : NULL));
  } else if (a == &t->c) {
    return (b == &t->b ? &t->a : (b == &t->a ? &t->b : NULL));
  }
  return NULL;
}

ct_export CT_Vec2f *ct_triangle2f_point_opposite(CT_Triangle2f *t,
                                                 const CT_Vec2f *a,
                                                 const CT_Vec2f *b) {
  if (ct_deltaeq2fv(a, &t->a, EPS)) {
    return (ct_deltaeq2fv(b, &t->b, EPS)
                ? &t->c
                : (ct_deltaeq2fv(b, &t->c, EPS) ? &t->b : NULL));
  } else if (ct_deltaeq2fv(a, &t->b, EPS)) {
    return (ct_deltaeq2fv(b, &t->a, EPS)
                ? &t->c
                : (ct_deltaeq2fv(b, &t->c, EPS) ? &t->a : NULL));
  } else if (ct_deltaeq2fv(a, &t->c, EPS)) {
    return (ct_deltaeq2fv(b, &t->b, EPS)
                ? &t->a
                : (ct_deltaeq2fv(b, &t->a, EPS) ? &t->b : NULL));
  }
  return NULL;
}

ct_export CT_Triangle3f *ct_triangle3f_init(CT_Triangle3f *t,
                                            const CT_Vec3f *a,
                                            const CT_Vec3f *b,
                                            const CT_Vec3f *c) {
  t->a = *a;
  t->b = *b;
  t->c = *c;
  return t;
}

ct_export CT_Triangle3f *ct_triangle3f_initpv(CT_Triangle3f *t,
                                              const CT_Vec3f *points) {
  t->a = *points++;
  t->b = *points++;
  t->c = *points;
  return t;
}

ct_export CT_Triangle3f *ct_triangle3f_initpf(CT_Triangle3f *t,
                                              const float *coords) {
  t->a.x = *coords++;
  t->a.y = *coords++;
  t->a.z = *coords++;
  t->b.x = *coords++;
  t->b.y = *coords++;
  t->b.z = *coords++;
  t->c.x = *coords++;
  t->c.y = *coords++;
  t->c.z = *coords;
  return t;
}

ct_export CT_Vec3f *ct_triangle3f_normal(const CT_Triangle3f *t,
                                         CT_Vec3f *out,
                                         size_t normalize) {
  CT_Vec3f ba, ca;
  ct_cross3fv(ct_sub3fv(&t->b, &t->a, &ba), ct_sub3fv(&t->c, &t->a, &ca), out);
  if (normalize) {
    ct_normalize3f_imm(out, 1.f);
  }
  return out;
}

ct_export float ct_triangle3f_normsign(const CT_Triangle3f *t) {
  CT_Vec3f n;
  return ct_mag3f(ct_triangle3f_normal(t, &n, 0));
}

ct_export float ct_triangle3f_area(CT_Triangle3f *t) {
  return 0.5f * ct_triangle3f_normsign(t);
}

ct_export float ct_triangle3f_circumference(CT_Triangle3f *t) {
  return ct_dist3fv(&t->a, &t->b) + ct_dist3fv(&t->b, &t->c) +
         ct_dist3fv(&t->c, &t->a);
}

ct_export size_t ct_triangle3f_is_clockwise(const CT_Triangle3f *t,
                                            const CT_Vec3f *n) {
  CT_Vec3f tn;
  return ct_dot3fv(ct_triangle3f_normal(t, &tn, 0), n) > 0.f;
}

ct_export CT_Vec3f *ct_triangle3f_point_opposite_ptr(CT_Triangle3f *t,
                                                     const CT_Vec3f *a,
                                                     const CT_Vec3f *b) {
  if (a == &t->a) {
    return (b == &t->b ? &t->c : (b == &t->c ? &t->b : NULL));
  } else if (a == &t->b) {
    return (b == &t->a ? &t->c : (b == &t->c ? &t->a : NULL));
  } else if (a == &t->c) {
    return (b == &t->b ? &t->a : (b == &t->a ? &t->b : NULL));
  }
  return NULL;
}

ct_export CT_Vec3f *ct_triangle3f_point_opposite(CT_Triangle3f *t,
                                                 const CT_Vec3f *a,
                                                 const CT_Vec3f *b) {
  if (ct_deltaeq3fv(a, &t->a, EPS)) {
    return (ct_deltaeq3fv(b, &t->b, EPS)
                ? &t->c
                : (ct_deltaeq3fv(b, &t->c, EPS) ? &t->b : NULL));
  } else if (ct_deltaeq3fv(a, &t->b, EPS)) {
    return (ct_deltaeq3fv(b, &t->a, EPS)
                ? &t->c
                : (ct_deltaeq3fv(b, &t->c, EPS) ? &t->a : NULL));
  } else if (ct_deltaeq3fv(a, &t->c, EPS)) {
    return (ct_deltaeq3fv(b, &t->b, EPS)
                ? &t->a
                : (ct_deltaeq3fv(b, &t->a, EPS) ? &t->b : NULL));
  }
  return NULL;
}
