#ifndef __ALGORITHM_ASTAR_H
#define __ALGORITHM_ASTAR_H
#include "struct/bool.h"
#include "struct/point.h"
#include "struct/tile.h"
#include <stddef.h>

#define ASTAR_STANDARD_UNIT 5.0
#define ASTAR_PARALLEL_COST 5
#define ASTAR_DIAGONAL_COST 7

typedef long aster_cost_t;
aster_cost_t direction_cost(direction_t d);
aster_cost_t astar_estimate_cost(point *from, point *to);

typedef enum __astar_result {
  ASTAR_INIT = 0,
  ASTAR_RUNNING,
  ASTAR_SUCCEEDED,
  ASTAR_FAILED
} astar_state;

typedef struct __astar_point_state {
  bool visited : 1;
  bool calculated : 1;
  bool used : 1;
  direction_t direction : 4;
  aster_cost_t paid_cost;
  aster_cost_t predict_cost;
} astar_point_state;

typedef struct __astar_context_struct {
  astar_state state;
  size_t iteration;
  point start_point;
  point end_point;
  tile_map map;
  point *queue;
  point *queue_start;
  point *queue_end;
  astar_point_state states[];
} *astar_context;

astar_context astar_init(const tile_map map, point start, point end);
void astar_free(astar_context *astar_ptr);

astar_state astar_resolve(astar_context astar);
void astar_print(const astar_context astar, FILE *f);

#endif
