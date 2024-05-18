#include "algorithm/astar.h"
#include "struct/point.h"
#include "struct/tile.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

tile_map generate_tile_map(size_t rows, size_t cols) {
  tile_map map = tile_map_new(rows, cols);
  for (size_t r = 0; r < rows; r++) {
    for (size_t c = 0; c < cols; c++) {
      int rand_val = (unsigned)random() % 3;
      tile_t val = tile_from_int(rand_val);
      tile_map_set(map, r, c, val);
    }
  }
  return map;
}

point generate_empty_point(tile_map map, point *except) {
  point pt;
  for (size_t i = 0; i < 1000; i++) {
    pt.row = (unsigned)random() % map->rows;
    pt.col = (unsigned)random() % map->cols;
    if (tile_map_get(map, pt.row, pt.col) != TILE_EMPTY) {
      continue;
    }
    if (except && point_equal(pt, *except)) {
      continue;
    }
    return pt;
  }
  pt.row = map->rows;
  pt.col = map->cols;
  return pt;
}

int main() {
  long seed = 1716011502;
  // long seed = time(NULL);
  srandom(seed);
  printf("seed: %ld\n", seed);
  tile_map map = generate_tile_map(10, 10);
  tile_map_print(map, stdout);
  point start_point = generate_empty_point(map, NULL);
  if (!tile_map_contains(map, start_point)) {
    printf("seed: %ld\n", seed);
    return EXIT_FAILURE;
  }
  printf("start point is (%zu, %zu)\n", start_point.row, start_point.col);
  point end_point = generate_empty_point(map, &start_point);
  if (!tile_map_contains(map, end_point)) {
    printf("seed: %ld\n", seed);
    return EXIT_FAILURE;
  }
  printf("end point is (%zu, %zu)\n", end_point.row, end_point.col);

  astar_context astar = astar_init(map, start_point, end_point);
  astar_print(astar, stdout);
  astar_resolve(astar);

  printf("seed: %ld\n", seed);
  return EXIT_SUCCESS;
}
