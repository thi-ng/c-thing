#pragma once

#define CT_VERSION_MAJOR 0
#define CT_VERSION_MINOR 1

//#define CT_FEATURE_CHECKS
//#define CT_FEATURE_LOG
//#define CT_FEATURE_TRACE_MPOOL
//#define CT_FEATURE_SSE

#if (defined(CT_FEATURE_CHECKS) || defined(CT_FEATURE_TRACE_MPOOL)) && \
    !defined(CT_FEATURE_LOG)
#define CT_FEATURE_LOG
#endif

#if defined(CT_FEATURE_SSE) && !defined(__SSE__)
#undef CT_FEATURE_SSE
#endif

#if defined(CT_FEATURE_SSE) && defined(__SSE2__)
#define CT_FEATURE_SSE2
#endif

#ifdef __EMSCRIPTEN__
#define CT_EXPORT __attribute__((used))
#else
#define CT_EXPORT
#endif

#define ct_inline static inline
