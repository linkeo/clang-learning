#ifndef __STRUCT_POINT_H
#define __STRUCT_POINT_H
#include "struct/bool.h"
#include <stddef.h>

typedef unsigned char direction_t;

#define DIRECTION_NORTH_WEST 0
#define DIRECTION_NORTH 1
#define DIRECTION_NORTH_EAST 2
#define DIRECTION_WEST 3
#define DIRECTION_EAST 5
#define DIRECTION_SOUTH_WEST 6
#define DIRECTION_SOUTH 7
#define DIRECTION_SOUTH_EAST 8
#define DIRECTION_LENGTH 8

typedef struct __point_struct {
  size_t row;
  size_t col;
} point;

char *direction_str(direction_t d);
direction_t direction_reverse(direction_t direction);
point point_move(point from, direction_t direction);
bool point_equal(point from, point to);
direction_t point_move_direction(point from, point to);
direction_t *direction_start();
direction_t *direction_end();
direction_t *direction_next(direction_t *ptr);

#endif
