#include "glfw_example.h"

char *slurp(const char *path) {
  FILE *f = fopen(path, "r");
  if (!f)
    return NULL;
  fseek(f, 0, SEEK_END);
  size_t len = ftell(f);
  char *buf  = malloc(len);
  fseek(f, 0, SEEK_SET);
  fread(buf, 1, len, f);
  fclose(f);
  return buf;
}

float *read_stl_binary(const char *path, uint32_t *num) {
  CT_INFO("loading stl: %s", path);

  FILE *f = fopen(path, "rb");
  CT_CHECK(f, "can't open file...");

  fseek(f, 80, SEEK_SET);
  fread(num, 4, 1, f);
  uint32_t numf = *num;
  CT_INFO("%u faces", numf);

  // 3 verts + 3 normals
  float *buf = calloc(numf, sizeof(float) * 18);

  // skip 1st normal
  fseek(f, 3 * sizeof(float), SEEK_CUR);

  for (uint32_t i = 0; i < numf; i++) {
    const size_t j = i * 18;
    CT_Vec3f a, b, c, n;

    fread(&buf[j], sizeof(float), 3, f);
    fread(&buf[j + 6], sizeof(float), 3, f);
    fread(&buf[j + 12], sizeof(float), 3, f);
    // skip face attrib + next normal
    fseek(f, 2 + 3 * sizeof(float), SEEK_CUR);

#ifdef CT_FEATURE_SSE
    // memcpy needed for SSE alignment
    memcpy(&a, &buf[j], 3 * sizeof(float));
    memcpy(&b, &buf[j + 6], 3 * sizeof(float));
    memcpy(&c, &buf[j + 12], 3 * sizeof(float));
    ct_orthonormal3fv(&a, &b, &c, &n);
#else
    ct_orthonormal3fv((CT_Vec3f *)&buf[j], (CT_Vec3f *)&buf[j + 6],
                      (CT_Vec3f *)&buf[j + 12], &n);
#endif

    memcpy(&buf[j + 3], &n, 3 * sizeof(float));
    memcpy(&buf[j + 9], &n, 3 * sizeof(float));
    memcpy(&buf[j + 15], &n, 3 * sizeof(float));

    CT_DEBUG("[%f,%f,%f] [%f,%f,%f] [%f,%f,%f] n: [%f,%f,%f]", buf[j],
             buf[j + 1], buf[j + 2], buf[j + 6], buf[j + 7], buf[j + 8],
             buf[j + 12], buf[j + 13], buf[j + 14], n.x, n.y, n.z);
  }

  fclose(f);
  return buf;
fail:
  exit(1);
}

GLuint init_shader(GLenum type, const char *src) {
  static char log[0x400];
  int status;
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);
  glGetShaderInfoLog(shader, 0x400, &status, log);
  CT_CHECK(!status, "shader error: %s", log);
  return shader;
fail:
  exit(1);
}

GLuint init_shader_program(const char *vsrc, const char *fsrc) {
  GLuint vs      = init_shader(GL_VERTEX_SHADER, vsrc);
  GLuint fs      = init_shader(GL_FRAGMENT_SHADER, fsrc);
  GLuint program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);

  int status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  glDeleteShader(vs);
  glDeleteShader(fs);
  CT_CHECK(!status, "link error: %d");
  return program;
fail:
  exit(1);
}
