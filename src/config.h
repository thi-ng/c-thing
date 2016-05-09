#pragma once

#define CT_VERSION_MAJOR 0
#define CT_VERSION_MINOR 1

#if !defined(NO_SSE) && defined(__SSE__)
#define CT_USE_SSE
#endif
