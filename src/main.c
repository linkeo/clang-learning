#include "algorithm/astar.h"
#include "algorithm/astar_draw_image.h"
#include "image/bitmap.h"
#include "struct/point.h"
#include "struct/tile.h"
#include "util/debug.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

size_t MAP_ROWS = 270;
size_t MAP_COLS = 480;
double EMPTY_RATIO = 0.54321;

tile_map generate_tile_map(size_t rows, size_t cols) {
  tile_map map = tile_map_new(rows, cols);
  for (size_t r = 0; r < rows; r++) {
    for (size_t c = 0; c < cols; c++) {
      bool is_empty = (double)((unsigned)random() % 1000) / 1000 < EMPTY_RATIO;
      tile_t val = tile_from_int(is_empty ? TILE_EMPTY : TILE_WALL);
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
  debugf("failed to generate empty point in %d tries.\n", 1000);
  pt.row = map->rows;
  pt.col = map->cols;
  return pt;
}

double current_time() {
  struct timespec ts;
  timespec_get(&ts, TIME_UTC);
  return ts.tv_sec * 1000 + (double)ts.tv_nsec / 1000000;
}

int main() {
  unsigned seed = (long)current_time();
  // unsigned seed = 283098000;
  srandom(seed);
  printf("seed: %u\n", seed);

  tile_map map = generate_tile_map(MAP_ROWS, MAP_COLS);

  FILE *map_file = fopen("astar_map.generated.bmp", "wb");
  bitmap_image map_image = tile_map_draw_image(map);
  bitmap_image_write(map_image, map_file);
  fclose(map_file);
  bitmap_free(&map_image);

  point start_point = generate_empty_point(map, NULL);
  if (!tile_map_contains(map, start_point)) {
    printf("seed: %u\n", seed);
    return EXIT_FAILURE;
  }
  printf("start point is (%zu, %zu)\n", start_point.row, start_point.col);
  point end_point = generate_empty_point(map, &start_point);
  if (!tile_map_contains(map, end_point)) {
    printf("seed: %u\n", seed);
    return EXIT_FAILURE;
  }
  printf("end point is (%zu, %zu)\n", end_point.row, end_point.col);

  double time_before_init = current_time();
  astar_context astar = astar_init(map, start_point, end_point);
  double time_after_init = current_time();
  double time_cost_init = time_after_init - time_before_init;
  printf("astar init time: %.3fms = %.1f times/frame\n", time_cost_init,
         50 / time_cost_init / 3);

  double time_before_resolve = current_time();
  astar_resolve(astar);
  double time_after_resolve = current_time();
  // astar_print(astar, stdout);
  printf("astar iteration: %zu, path length: %zu\n", astar->iteration,
         astar->path_length);
  aster_cost_t estimate_cost = astar_estimate_cost(&start_point, &end_point);
  printf("astar comparison times: %zu, ideal free cost x %.3f\n",
         astar->comparison_count,
         (double)astar->comparison_count / (estimate_cost * estimate_cost));
  printf("estimate cost: %.1f\n", (double)estimate_cost);
  printf("actual cost: %.1f, extra cost: %.1f%%\n", (double)astar->path_cost,
         ((double)astar->path_cost / estimate_cost - 1) * 100);
  printf("actual iter: %.1f, iter ratio: %.3f\n", (double)astar->iteration,
         (double)astar->iteration / estimate_cost);
  double time_cost_resolve = time_after_resolve - time_before_resolve;
  printf("astar resolve time: %.3fms = %.1f times/frame\n", time_cost_resolve,
         50 / time_cost_resolve / 3);

  printf("seed: %u\n", seed);
  printf("map: %zu x %zu = %zu blocks\n", MAP_ROWS, MAP_COLS,
         MAP_ROWS * MAP_COLS);

  FILE *result_file = fopen("astar_result.generated.bmp", "wb");
  bitmap_image result_image = astar_draw_image(astar);
  bitmap_image_write(result_image, result_file);
  fclose(result_file);
  bitmap_free(&result_image);

  return EXIT_SUCCESS;
}
