#include "algorithm/astar_draw_image.h"
#include "algorithm/astar.h"
#include "image/bitmap.h"
#include "struct/tile.h"
#include "util/debug.h"
#include <stddef.h>
#include <stdio.h>

#define BORDER_SIZE 5
#define TILE_SIZE 3
#define BORDER_COLOR BITMAP_BLACK

static void tile_map_draw_line(bitmap_image image, size_t y, const tile_map map,
                               size_t row);
static void tile_map_draw_border(bitmap_image image, size_t x, size_t y,
                                 size_t width, size_t height);
static void tile_map_draw_tile(bitmap_image image, size_t x, size_t y,
                               tile_t tile);

bitmap_image tile_map_draw_image(const tile_map map) {
  debugf("tile_map_draw_image start\n");

  size_t width = map->cols * TILE_SIZE + 2 * BORDER_SIZE;
  size_t height = map->rows * TILE_SIZE + 2 * BORDER_SIZE;
  bitmap_image image = bitmap_new(width, height);
  debugf("tile_map_draw_image image created\n");

  /// top border
  tile_map_draw_border(image, 0, 0, width, BORDER_SIZE);
  debugf("tile_map_draw_image top border done\n");

  for (size_t row = 0; row < map->rows; row++) {
    tile_map_draw_line(image, BORDER_SIZE + row * TILE_SIZE, map, row);
  }

  /// bottom border
  tile_map_draw_border(image, 0, BORDER_SIZE + map->rows * TILE_SIZE, width,
                       BORDER_SIZE);
  debugf("tile_map_draw_image bottom border done\n");

  debugf("tile_map_draw_image end\n");
  return image;
}

static void astar_draw_line(bitmap_image image, size_t y,
                            const astar_context astar, size_t row);
static void astar_draw_tile(bitmap_image image, size_t x, size_t y,
                            const astar_context astar, size_t row, size_t col);

bitmap_image astar_draw_image(const astar_context astar) {
  debugf("astar_draw_image start\n");
  tile_map map = astar->map;

  size_t width = map->cols * TILE_SIZE + 2 * BORDER_SIZE;
  size_t height = map->rows * TILE_SIZE + 2 * BORDER_SIZE;
  bitmap_image image = bitmap_new(width, height);
  debugf("astar_draw_image image created\n");

  /// top border
  tile_map_draw_border(image, 0, 0, width, BORDER_SIZE);
  debugf("astar_draw_image top border done\n");

  for (size_t row = 0; row < map->rows; row++) {
    astar_draw_line(image, BORDER_SIZE + row * TILE_SIZE, astar, row);
  }

  /// bottom border
  tile_map_draw_border(image, 0, BORDER_SIZE + map->rows * TILE_SIZE, width,
                       BORDER_SIZE);
  debugf("astar_draw_image bottom border done\n");

  debugf("astar_draw_image end\n");
  return image;
}

static void tile_map_draw_line(bitmap_image image, size_t y, const tile_map map,
                               size_t row) {

  /// left border
  tile_map_draw_border(image, 0, y, BORDER_SIZE, TILE_SIZE);
  size_t x = BORDER_SIZE;

  for (size_t col = 0; col < map->cols; col++, x += TILE_SIZE) {
    tile_map_draw_tile(image, x, y, tile_map_get(map, row, col));
  }

  /// right border
  tile_map_draw_border(image, x, y, BORDER_SIZE, TILE_SIZE);
  x += BORDER_SIZE;
}

static void astar_draw_line(bitmap_image image, size_t y,
                            const astar_context astar, size_t row) {
  tile_map map = astar->map;

  /// left border
  tile_map_draw_border(image, 0, y, BORDER_SIZE, TILE_SIZE);
  size_t x = BORDER_SIZE;

  for (size_t col = 0; col < map->cols; col++, x += TILE_SIZE) {
    astar_draw_tile(image, x, y, astar, row, col);
  }

  /// right border
  tile_map_draw_border(image, x, y, BORDER_SIZE, TILE_SIZE);
  x += BORDER_SIZE;
}

static void tile_map_draw_border(bitmap_image image, size_t x, size_t y,
                                 size_t width, size_t height) {
  for (size_t dx = 0; dx < width; dx++) {
    for (size_t dy = 0; dy < height; dy++) {
      bitmap_set_color(image, x + dx, y + dy, BORDER_COLOR);
    }
  }
}

static inline void draw_tile(bitmap_image image, size_t x, size_t y,
                             bitmap_color color);
static inline void tile_map_draw_tile(bitmap_image image, size_t x, size_t y,
                                      tile_t tile) {
  static bitmap_color tile_colors[TILE_INVALID + 1] = {
      BITMAP_WHITE,              /// EMPTY
      bitmap_rgb(127, 127, 127), /// WALL
      BITMAP_MAGENTA             /// INVALID
  };
  draw_tile(image, x, y, tile_colors[tile]);
}

static inline void draw_tile(bitmap_image image, size_t x, size_t y,
                             bitmap_color color) {
  for (size_t dx = 0; dx < TILE_SIZE; dx++) {
    for (size_t dy = 0; dy < TILE_SIZE; dy++) {
      bitmap_set_color(image, x + dx, y + dy, color);
    }
  }
}

static inline void astar_draw_tile(bitmap_image image, size_t x, size_t y,
                                   const astar_context astar, size_t row,
                                   size_t col) {
  static bitmap_color tile_colors[ASTAR_VISITED + 1] = {
      BITMAP_WHITE,              /// ASTAR_ORIGINAL
      bitmap_rgb(255, 127, 127), /// ASTAR_START_POINT
      bitmap_rgb(127, 0, 0),     /// ASTAR_END_POINT
      BITMAP_RED,                /// ASTAR_PATH
      BITMAP_YELLOW,             /// ASTAR_MARKED
      bitmap_rgb(255, 192, 0),   /// ASTAR_VISITED
  };
  astar_point_type point_type = astar_get_point_type(astar, row, col);
  switch (point_type) {
  case ASTAR_ORIGINAL:
    tile_map_draw_tile(image, x, y, tile_map_get(astar->map, row, col));
    break;
  default:
    draw_tile(image, x, y, tile_colors[point_type]);
    break;
  }
}
