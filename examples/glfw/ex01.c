#include "glfw_example.h"

extern const char* phong_vs_src;
extern const char* phong_fs_src;

GLFWwindow* window;
GLuint vbo, program;
GLint model_loc, nmat_loc, view_loc, proj_loc;

double mpos[] = {320, 240};
double zoom   = 1;
bool is_fullsceen;
float* vertices;
uint32_t numFaces;

static void error_callback(int error, const char* description) {
  CT_ERROR("Error: %s", description);
}

static void key_callback(GLFWwindow* window,
                         int key,
                         int scancode,
                         int action,
                         int mods) {
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_ESCAPE) {
      glfwSetWindowShouldClose(window, 1);
    }
#ifndef __EMSCRIPTEN__
    if (key == GLFW_KEY_F) {
      is_fullsceen = !is_fullsceen;
      if (is_fullsceen) {
        glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, 1280, 720,
                             60);
      } else {
        glfwSetWindowMonitor(window, NULL, 100, 100, 640, 480, 60);
        glfwSetWindowSize(window, 640, 480);
      }
    }
#endif
  }
}

static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
  mpos[0] = xpos;
  mpos[1] = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  zoom += yoffset * 0.01;
  zoom = ct_clampf(zoom, 0.1, 4);
  CT_DEBUG("%f", zoom);
}

void render() {
  int width, height;
  float aspect;
  CT_Mat4f model, proj, view, nmat, t;

  glfwGetFramebufferSize(window, &width, &height);
  aspect = (width / (float)height);
  glViewport(0, 0, width, height);
  glClearColor(0.1, 0.1, 0.1, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  float mx = (width / 2.0 - mpos[0]) / width * 2 * aspect;
  float my = (height / 2.0 - mpos[1]) / height * 2;
  ct_mat4f_set_identity(&model);
  ct_mat4f_translate3fp_imm(&model, FVEC(mx, my, 0));
  ct_mat4f_rotatex_imm(&model, (float)glfwGetTime() * 0.5);
  ct_mat4f_rotatey_imm(&model, (float)glfwGetTime());
  ct_mat4f_set_identity(&t);
  ct_mat4f_scalen_imm(&t, zoom);
  ct_mat4f_mul_imm(&model, &t);
  CT_Vec3f eye    = {0, 0, -2};
  CT_Vec3f target = {0, 0, 0};
  CT_Vec3f up     = {0, 1, 0};
  ct_mat4f_set_lookat(&view, &eye, &target, &up);
  ct_mat4f_set_perspective(&proj, 60, aspect, 0.1, 10);

  ct_mat4f_transpose_imm(
      ct_mat4f_invert(ct_mat4f_mul(&view, &model, &t), &nmat));

  glUseProgram(program);
  glUniformMatrix4fv(model_loc, 1, GL_FALSE, (GLfloat*)&model);
  glUniformMatrix4fv(view_loc, 1, GL_FALSE, (GLfloat*)&view);
  glUniformMatrix4fv(proj_loc, 1, GL_FALSE, (GLfloat*)&proj);
  glUniformMatrix4fv(nmat_loc, 1, GL_FALSE, (GLfloat*)&nmat);
  glUniform3f(glGetUniformLocation(program, "lightPos"), 0, 1, 2);
  glUniform3f(glGetUniformLocation(program, "ambientCol"), 0.05, 0.1, 0.2);
  glUniform3f(glGetUniformLocation(program, "diffuseCol"), 0.05, 0.8, 1);
  glUniform3f(glGetUniformLocation(program, "specularCol"), 1, 1, 1);
  glUniform1f(glGetUniformLocation(program, "shininess"), 16);

  glDrawArrays(GL_TRIANGLES, 0, numFaces * 3);
}

int main(void) {
  char log[1024];
  int status = 0;

  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) {
    return 1;
  }
  //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_RESIZABLE, 1);

  window = glfwCreateWindow(640, 480, "c.thi.ng x GLFW", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
#ifndef __EMSCRIPTEN__
  glfwSetCursor(window, glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR));
#endif
  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, cursor_pos_callback);
  glfwSetScrollCallback(window, scroll_callback);

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  CT_TIMED(vertices = read_stl_binary(ASSET_PATH "/suzanne.stl", &numFaces));
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, numFaces * 18 * sizeof(float), vertices,
               GL_STATIC_DRAW);

  program = init_shader_program(phong_vs_src, phong_fs_src);

  model_loc = glGetUniformLocation(program, "model");
  view_loc  = glGetUniformLocation(program, "view");
  proj_loc  = glGetUniformLocation(program, "proj");
  nmat_loc  = glGetUniformLocation(program, "normalMat");
  CT_DEBUG("model: %d, view: %d, proj: %d, nmat: %d", model_loc, view_loc,
           proj_loc, nmat_loc);

  GLint pos    = glGetAttribLocation(program, "position");
  GLint normal = glGetAttribLocation(program, "normal");
  CT_DEBUG("pos: %d, norm: %d", pos, normal);

  glEnableVertexAttribArray(pos);
  glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void*)0);
  glEnableVertexAttribArray(normal);
  glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void*)(sizeof(float) * 3));
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(render, 0, 1);
#else
  while (!glfwWindowShouldClose(window)) {
    render();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwDestroyWindow(window);
#endif
  glfwTerminate();
  free(vertices);
  return 0;
}
