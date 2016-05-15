// based on: http://c.learncodethehardway.org/book/ex20.html

#pragma once

#include "config.h"

#if defined(CT_FEATURE_CHECKS) || defined(CT_FEATURE_LOG)
#include <errno.h>
#include <stdio.h>
#include <string.h>
#endif

#ifdef CT_FEATURE_LOG

#ifdef NDEBUG
#define CT_DEBUG(M, ...)
#else
#define CT_DEBUG(M, ...) \
  fprintf(stderr, "[DEBUG] %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define CT_CLEAN_ERRNO() (errno == 0 ? "None" : strerror(errno))

#define CT_ERROR(M, ...)                                                    \
  fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, \
          CT_CLEAN_ERRNO(), ##__VA_ARGS__)

#define CT_WARN(M, ...)                                                    \
  fprintf(stderr, "[WARN] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, \
          CT_CLEAN_ERRNO(), ##__VA_ARGS__)

#define CT_INFO(M, ...) \
  fprintf(stderr, "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define CT_SENTINEL(M, ...)     \
  {                             \
    CT_ERROR(M, ##__VA_ARGS__); \
    errno = 0;                  \
    goto fail;                  \
  }

#else
#define CT_DEBUG(M, ...)
#define CT_CLEAN_ERRNO()
#define CT_ERROR(M, ...)
#define CT_WARN(M, ...)
#define CT_INFO(M, ...)
#define CT_SENTINEL(M, ...) \
  {}
#endif

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

#define CT_CHECK_MEM(A) CT_CHECK((A), "Out of memory!")
#else
#define CT_CHECK(A, M, ...)
#define CT_CHECK_DEBUG(A, M, ...)
#define CT_CHECK_MEM(A)
#endif
