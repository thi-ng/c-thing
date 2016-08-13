#pragma once

#define CT_VERSION_MAJOR 0
#define CT_VERSION_MINOR 1

#include "ct-head/features.h"
#include "ct-head/log.h"

#ifndef CT_MAX_TYPES
#define CT_MAX_TYPES 16
#endif

#ifndef CT_POOLSIZE_OBJECT
#define CT_POOLSIZE_OBJECT 0x1000
#endif

#ifndef CT_POOLSIZE_CONSRC
#define CT_POOLSIZE_CONSRC 0x1000
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
