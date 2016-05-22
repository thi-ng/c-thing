#pragma once

#define CT_VERSION_MAJOR 0
#define CT_VERSION_MINOR 1

//#define CT_FEATURE_CHECKS
//#define CT_FEATURE_LOG
//#define CT_FEATURE_SSE

#ifdef CT_FEATURE_CHECKS
#define CT_FEATURE_LOG
#endif

#if defined(CT_FEATURE_SSE) && !defined(__SSE__)
#undef CT_FEATURE_SSE
#endif

#if defined(CT_FEATURE_SSE) && defined(__SSE2__)
#define CT_FEATURE_SSE2
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
#define CT_EXPORT __attribute__((used))
#else
#define CT_EXPORT
#endif

#define ct_inline static inline
