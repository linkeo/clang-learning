#ifndef __STRUCT_TILE_H
#define __STRUCT_TILE_H
#include "struct/point.h"
#include <stddef.h>
#include <stdio.h>

typedef enum __tile_t_enum {
  TILE_EMPTY = 0,
  TILE_WALL,

  // last is invalid
  TILE_INVALID
} tile_t;

typedef struct __tile_map_struct {
  size_t rows;
  size_t cols;
  tile_t tiles[];
} *tile_map;

char *tile_str(tile_t value);
tile_t tile_from_int(int i);
tile_t tile_from_char(char c);

tile_map tile_map_new(size_t rows, size_t cols);
void tile_map_free(tile_map *map_ptr);
void tile_map_print(const tile_map map, FILE *f);

tile_t tile_map_get(const tile_map map, size_t row, size_t col);
void tile_map_set(tile_map map, size_t row, size_t col, tile_t value);

size_t tile_map_pos(const tile_map map, size_t row, size_t col);
tile_t tile_map_pos_get(const tile_map map, size_t pos);
void tile_map_pos_set(tile_map map, size_t pos, tile_t value);

bool tile_map_contains(tile_map map, point pt);

#endif
