#pragma once

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GLFW_INCLUDE_ES2
#endif

#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "math/matrix.h"
#include "math/vec.h"

#include "linmath.h"

#ifndef ASSET_PATH
#ifdef __EMSCRIPTEN__
#define ASSET_PATH "assets"
#else
#define ASSET_PATH "assets"
#endif
#endif

char* slurp(const char* path);
float* read_stl_binary(const char* path, uint32_t* numf);

GLuint init_shader(GLenum type, const char* src);
GLuint init_shader_program(const char* vsrc, const char* fsrc);
