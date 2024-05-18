#ifndef __ALGORITHM_ASTAR_DRAW_IMAGE_H
#define __ALGORITHM_ASTAR_DRAW_IMAGE_H

#include "algorithm/astar.h"
#include "image/bitmap.h"
#include "struct/tile.h"

bitmap_image tile_map_draw_image(const tile_map map);
bitmap_image astar_draw_image(const astar_context astar);

#endif
