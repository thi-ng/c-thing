#pragma once

#define CT_VERSION_MAJOR 0
#define CT_VERSION_MINOR 1

#if defined(CT_FEATURE_CHECKS) && !defined(CT_FEATURE_LOG)
#define CT_FEATURE_LOG 1
#endif

#if defined(CT_FEATURE_SSE) && !defined(__SSE__)
#undef CT_FEATURE_SSE 1
#endif

#if defined(CT_FEATURE_SSE) && defined(__SSE2__)
#define CT_FEATURE_SSE2 1
#endif

#if defined(CT_FEATURE_SSE) && defined(__SSE3__)
#define CT_FEATURE_SSE3 1
#endif

#if defined(CT_FEATURE_SSE) && defined(__SSE4_1__)
#define CT_FEATURE_SSE4 1
#endif

#ifdef CT_FEATURE_ANSI
#define CT_ANSI_RED "\x1b[31;1m"
#define CT_ANSI_GREEN "\x1b[32;1m"
#define CT_ANSI_RESET "\x1b[0m"
#else
#define CT_ANSI_RED
#define CT_ANSI_GREEN
#define CT_ANSI_RESET
#endif

#ifdef __EMSCRIPTEN__
#define ct_export __attribute__((used))
#else
#define ct_export
#endif

#define ct_inline static inline

#ifdef __cplusplus
#define CT_BEGIN_DECLS extern "C" {
#define CT_END_DECLS }
#else
#define CT_BEGIN_DECLS
#define CT_END_DECLS
#endif

#ifndef CT_MAX_TYPES
#define CT_MAX_TYPES 16
#endif

#ifndef CT_POOLSIZE_OBJECT
#define CT_POOLSIZE_OBJECT 0x1000
#endif

#ifndef CT_POOLSIZE_CONSRC
#define CT_POOLSIZE_CONSRC 0x1000
#endif

////////////////////////////////////////////////////////////
// based on: http://c.learncodethehardway.org/book/ex20.html

#include <errno.h>
#include <time.h>

#if defined(CT_FEATURE_CHECKS) || defined(CT_FEATURE_LOG)
#include <stdio.h>
#include <string.h>
#endif

////////////////////////////////////////////////////////////

#ifdef CT_FEATURE_LOG

#ifdef NDEBUG
#define CT_DEBUG(M, ...)
#else
#define CT_DEBUG(M, ...) \
  fprintf(stderr, "[DEBUG] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif  // NDEBUG

#define CT_FMT_ERRNO() (errno == 0 ? "n/a" : strerror(errno))

#define CT_ERROR(M, ...)                                                \
  fprintf(stderr,                                                       \
          CT_ANSI_RED "[ERROR] (%s:%d, err: %s) " M CT_ANSI_RESET "\n", \
          __FILE__, __LINE__, CT_FMT_ERRNO(), ##__VA_ARGS__)

#define CT_WARN(M, ...)                                                  \
  fprintf(stderr, "[WARN] (%s:%d: err: %s) " M "\n", __FILE__, __LINE__, \
          CT_FMT_ERRNO(), ##__VA_ARGS__)

#define CT_INFO(M, ...) \
  fprintf(stderr, "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define CT_SUCCESS(M, ...)                                                 \
  fprintf(stderr, CT_ANSI_GREEN "[SUCCESS] (%s:%d) " M CT_ANSI_RESET "\n", \
          __FILE__, __LINE__, ##__VA_ARGS__)

#else

#define CT_DEBUG(M, ...)
#define CT_FMT_ERRNO()
#define CT_ERROR(M, ...)
#define CT_WARN(M, ...)
#define CT_INFO(M, ...)
#define CT_SUCCESS(M, ...)

#endif  // CT_FEATURE_LOG

////////////////////////////////////////////////////////////

#ifdef CT_FEATURE_CHECKS

#define CT_CHECK(A, M, ...)     \
  if (!(A)) {                   \
    CT_ERROR(M, ##__VA_ARGS__); \
    errno = 0;                  \
    goto fail;                  \
  }

#define CT_CHECK_DEBUG(A, M, ...) \
  if (!(A)) {                     \
    CT_DEBUG(M, ##__VA_ARGS__);   \
    errno = 0;                    \
    goto fail;                    \
  }

#else

#define CT_CHECK(A, M, ...)
#define CT_CHECK_DEBUG(A, M, ...)

#endif  // CT_FEATURE_CHECKS

////////////////////////////////////////////////////////////

#if CT_FEATURE_CHECK_MEM

#define CT_CHECK_MEM(A)         \
  if (!(A)) {                   \
    CT_ERROR("Out of memory!"); \
    errno = 0;                  \
    goto fail;                  \
  }

#else

#define CT_CHECK_MEM(A)

#endif

////////////////////////////////////////////////////////////

#define CT_SENTINEL(M, ...)     \
  {                             \
    CT_ERROR(M, ##__VA_ARGS__); \
    errno = 0;                  \
    goto fail;                  \
  }

#define CT_TIMED(...)                                                   \
  {                                                                     \
    clock_t t0 = clock();                                               \
    __VA_ARGS__;                                                        \
    CT_INFO("time: %f", (double)(clock() - t0) / CLOCKS_PER_SEC * 1e3); \
  }
