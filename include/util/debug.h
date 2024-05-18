#ifndef __UTIL_DEBUG_H
#define __UTIL_DEBUG_H

// #define DEBUG

#ifdef DEBUG
#define DEBUG_ENABLED 1
#include "stdio.h"
#include "stdlib.h"
#define debugf(...)                                                            \
  do {                                                                         \
    fprintf(stderr, __VA_ARGS__);                                              \
  } while (false)
#else
#define DEBUG_ENABLED 0
#define debugf(...)                                                            \
  do {                                                                         \
  } while (false)
#endif

#endif
