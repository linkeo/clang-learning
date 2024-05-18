#include "struct/tile.h"
#include "struct/point.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

inline tile_t tile_from_int(int i) { return (unsigned)i % TILE_INVALID; }
inline tile_t tile_from_char(char c) { return tile_from_int((int)(c - '0')); }

char *tile_str(tile_t value) {
  static const char *str[] = {EMPTY_BLOCK, WALL_BLOCK, INVALID_BLOCK};
  return (char *)str[value];
}

tile_map tile_map_new(size_t rows, size_t cols) {
  tile_map ret;
  size_t tile_size = sizeof(tile_t) * rows * cols;
  ret = (tile_map)malloc(sizeof(*ret) + tile_size);
  memset(ret->tiles, 0, tile_size);
  ret->rows = rows;
  ret->cols = cols;
  return ret;
}

void tile_map_free(tile_map *map_ptr) {
  if (map_ptr) {
    free(*map_ptr);
    *map_ptr = NULL;
  }
}

void tile_map_print(const tile_map map, FILE *f) {
  if (!f) {
    f = stdout;
  }
  fprintf(f, "\n");
  fprintf(f, BORDER_BLOCK);
  for (size_t c = 0; c < map->cols; c++) {
    fprintf(f, BORDER_BLOCK);
  }
  fprintf(f, BORDER_BLOCK);
  fprintf(f, "\n");
  for (size_t r = 0; r < map->rows; r++) {
    fprintf(f, BORDER_BLOCK);
    for (size_t c = 0; c < map->cols; c++) {
      fprintf(f, "%3s", tile_str(tile_map_get(map, r, c)));
    }
    fprintf(f, BORDER_BLOCK);
    fprintf(f, "\n");
  }
  fprintf(f, BORDER_BLOCK);
  for (size_t c = 0; c < map->cols; c++) {
    fprintf(f, BORDER_BLOCK);
  }
  fprintf(f, BORDER_BLOCK);
  fprintf(f, "\n");
}

inline tile_t tile_map_get(const tile_map map, size_t row, size_t col) {
  return tile_map_pos_get(map, tile_map_pos(map, row, col));
}

inline void tile_map_set(tile_map map, size_t row, size_t col, tile_t value) {
  tile_map_pos_set(map, tile_map_pos(map, row, col),
                   (unsigned)value % TILE_INVALID);
}

inline size_t tile_map_pos(const tile_map map, size_t row, size_t col) {
  return map->cols * row + col;
}

inline tile_t tile_map_pos_get(const tile_map map, size_t pos) {
  return map->tiles[pos];
}

inline void tile_map_pos_set(tile_map map, size_t pos, tile_t value) {
  map->tiles[pos] = value;
}

bool tile_map_contains(tile_map map, point pt) {
  return pt.row >= 0 && pt.row < map->rows && pt.col >= 0 && pt.col < map->cols;
}
