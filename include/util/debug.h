#ifndef __UTIL_DEBUG_H
#define __UTIL_DEBUG_H

#define DEBUG

#ifdef DEBUG
#include "stdio.h"
#include "stdlib.h"
#define debugf(...)                                                            \
  do {                                                                         \
    fprintf(stderr, __VA_ARGS__);                                              \
  } while (false)
#else
#define debugf(...)                                                            \
  do {                                                                         \
  } while (false)
#endif

#endif
