// based on: http://c.learncodethehardway.org/book/ex20.html

#pragma once

#include <errno.h>
#include <time.h>

#include "config.h"

#if defined(CT_FEATURE_CHECKS) || defined(CT_FEATURE_LOG)
#include <stdio.h>
#include <string.h>
#endif

#ifdef CT_FEATURE_LOG

#ifdef NDEBUG
#define CT_DEBUG(M, ...)
#else
#define CT_DEBUG(M, ...) \
  fprintf(stderr, "[DEBUG] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif

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
