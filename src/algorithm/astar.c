#include "algorithm/astar.h"
#include "struct/point.h"
#include "struct/tile.h"
#include "util/debug.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>

void astar_enqueue(astar_context astar, const point *pt);
point *astar_dequeue(astar_context astar);
bool astar_queue_contains(astar_context astar, const point *pt);
void astar_iterate(astar_context astar);
void astar_peek_min_cost(astar_context astar);
astar_point_state *astar_point_ptr_by_pos(const astar_context astar, size_t row,
                                          size_t col);
astar_point_state *astar_point_ptr(const astar_context astar, point *pt);
int astar_push_next_points(astar_context astar, point pt);
void aster_calculate_point(astar_context astar, point *pt);

astar_context astar_init(tile_map map, point start, point end) {
  astar_context astar;
  size_t state_size = sizeof(astar_point_state) * map->rows * map->cols;
  astar = (astar_context)malloc(sizeof(*astar) + state_size);
  memset(astar->states, 0, state_size);
  astar->state = ASTAR_INIT;
  astar->iteration = 0;
  astar->start_point = start;
  astar->end_point = end;
  astar->map = map;
  astar->queue = (point *)malloc(sizeof(point) * map->rows * map->cols);
  astar->queue_start = astar->queue;
  astar->queue_end = astar->queue;
  return astar;
}

void astar_free(astar_context *astar_ptr) {
  if (astar_ptr) {
    astar_context astar = *astar_ptr;
    tile_map_free(&astar->map);
    if (astar->queue) {
      free(astar->queue);
      astar->queue = NULL;
    }
    free(astar);
    *astar_ptr = NULL;
  }
}

void astar_print(const astar_context astar, FILE *f) {
  if (!f) {
    f = stdout;
  }
  fprintf(f, "\n");
  fprintf(f, "||");
  for (size_t c = 0; c < astar->map->cols; c++) {
    fprintf(f, "===");
  }
  fprintf(f, "||");
  fprintf(f, "\n");
  point curr;
  for (size_t r = 0; r < astar->map->rows; r++) {
    fprintf(f, "||");
    curr.row = r;
    for (size_t c = 0; c < astar->map->cols; c++) {
      curr.col = c;
      if (point_equal(curr, astar->end_point)) {
        fprintf(f, "%3s", "(E)");
        continue;
      }
      if (point_equal(curr, astar->start_point)) {
        fprintf(f, "%3s", "(S)");
        continue;
      }
      astar_point_state *curr_state = astar_point_ptr(astar, &curr);
      if (curr_state->used) {
        fprintf(f, " %1s ", direction_str(curr_state->direction));
        continue;
      }
      if (curr_state->visited) {
        fprintf(f, "%3s", " x ");
        continue;
      }
      if (curr_state->calculated) {
        fprintf(f, "%3s", " + ");
        continue;
      }
      fprintf(f, "%3s", tile_str(tile_map_get(astar->map, r, c)));
    }
    fprintf(f, "||");
    fprintf(f, "\n");
  }
  fprintf(f, "||");
  for (size_t c = 0; c < astar->map->cols; c++) {
    fprintf(f, "===");
  }
  fprintf(f, "||");
  fprintf(f, "\n");
  fprintf(f, "astar iteration: %zu\n", astar->iteration);
}

astar_state astar_resolve(astar_context astar) {
  if (astar->state != ASTAR_INIT) {
    return astar->state;
  }
  debugf("\n===================\n");
  debugf("astar start running\n");
  astar->state = ASTAR_RUNNING;
  aster_calculate_point(astar, &astar->start_point);
  astar_enqueue(astar, &astar->start_point);
  while (astar->state == ASTAR_RUNNING) {
    astar_iterate(astar);
    astar_print(astar, stderr);
  }
  debugf("astar stop: %d\n", astar->state);
  return astar->state;
}

void astar_iterate(astar_context astar) {
  astar->iteration++;
  debugf("\n-------------------\n");
  debugf("astar start iteration %zu start\n", astar->iteration);
  astar_peek_min_cost(astar);
  point *pt = astar_dequeue(astar);
  if (!pt) {
    astar->state = ASTAR_FAILED;
    return;
  }
  astar_point_ptr(astar, pt)->visited = true;
  astar_push_next_points(astar, *pt);
  if (astar_queue_contains(astar, &astar->end_point)) {
    debugf("astar reach end point\n");
    int i = 0;
    for (point pt = astar->end_point;
         !point_equal(pt, astar->start_point) && i++ < 10;
         pt = point_move(
             pt, direction_reverse(astar_point_ptr(astar, &pt)->direction))) {
      debugf("astar mark path at (%zu, %zu)\n", pt.row, pt.col);
      astar_point_ptr(astar, &pt)->used = true;
    }
    astar_point_ptr(astar, &astar->start_point)->used = true;
    astar->state = ASTAR_SUCCEEDED;
  }
  debugf("astar start iteration %zu end\n", astar->iteration);
}

inline void astar_enqueue(astar_context astar, const point *pt) {
  debugf("astar enqueue (%zu, %zu)\n", pt->row, pt->col);
  *astar->queue_end++ = *pt;
}

inline point *astar_dequeue(astar_context astar) {
  if (astar->queue_start < astar->queue ||
      astar->queue_start >= astar->queue_end) {
    return NULL;
  }
  debugf("astar dequeue (%zu, %zu)\n", astar->queue_start->row,
         astar->queue_start->col);
  return astar->queue_start++;
}

aster_cost_t astar_compare_cost(astar_context astar, point *left,
                                point *right) {
  astar_point_state *left_state = astar_point_ptr(astar, left);
  astar_point_state *right_state = astar_point_ptr(astar, right);
  if (left_state->predict_cost != right_state->predict_cost) {
    return left_state->predict_cost - right_state->predict_cost;
  }
  if (left_state->paid_cost != right_state->paid_cost) {
    return left_state->paid_cost - right_state->paid_cost;
  }
  return 0;
}

void astar_peek_min_cost(astar_context astar) {
  if (astar->queue_start == astar->queue_end) {
    return;
  }
  point *min = NULL;
  for (point *curr = astar->queue_start; curr < astar->queue_end; curr++) {
    if (!min || astar_compare_cost(astar, curr, min) < 0) {
      min = curr;
    }
  }
  if (min != astar->queue_start) {
    point tmp = *min;
    *min = *astar->queue_start;
    *astar->queue_start = tmp;
  }
}

bool astar_queue_contains(astar_context astar, const point *pt) {
  for (point *scan = astar->queue_start; scan != astar->queue_end; scan++) {
    if (point_equal(*pt, *scan)) {
      return true;
    }
  }
  return false;
}

inline astar_point_state *astar_point_ptr_by_pos(astar_context astar,
                                                 size_t row, size_t col) {
  size_t offset = astar->map->cols * row + col;
  return astar->states + offset;
}

inline astar_point_state *astar_point_ptr(astar_context astar, point *pt) {
  return astar_point_ptr_by_pos(astar, pt->row, pt->col);
}

void aster_calculate_point(astar_context astar, point *pt) {
  astar_point_state *pt_state = astar_point_ptr(astar, pt);
  bool init = false;
  for (direction_t *d = direction_start(); d != direction_end();
       d = direction_next(d)) {
    point prev = point_move(*pt, direction_reverse(*d));
    if (!tile_map_contains(astar->map, prev)) {
      continue;
    }
    astar_point_state *prev_state = astar_point_ptr(astar, &prev);
    if (!prev_state->calculated) {
      continue;
    }
    aster_cost_t cost = prev_state->paid_cost + direction_cost(*d);
    if (!init || cost < pt_state->paid_cost) {
      init = true;
      pt_state->paid_cost = cost;
      pt_state->direction = *d;
    }
  }
  pt_state->predict_cost =
      pt_state->paid_cost + astar_estimate_cost(pt, &astar->end_point);
  pt_state->calculated = true;
  debugf(
      "astar calculate point %s (%zu, %zu) paid cost: %ld, predict cost: %ld\n",
      direction_str(pt_state->direction), pt->row, pt->col, pt_state->paid_cost,
      pt_state->predict_cost);
}

int astar_push_next_point(astar_context astar, point pt) {
  if (!tile_map_contains(astar->map, pt)) {
    return 0;
  }
  if (tile_map_get(astar->map, pt.row, pt.col) != TILE_EMPTY) {
    return 0;
  }
  if (astar_point_ptr(astar, &pt)->visited) {
    return 0;
  }
  if (!astar_queue_contains(astar, &pt)) {
    astar_enqueue(astar, &pt);
  }
  aster_calculate_point(astar, &pt);
  return 1;
}

int astar_push_next_points(astar_context astar, point pt) {
  int count = 0;
  for (direction_t *d = direction_start(); d != direction_end();
       d = direction_next(d)) {
    count += astar_push_next_point(astar, point_move(pt, *d));
  }
  return count;
}

inline aster_cost_t direction_cost(direction_t d) {
  return d % 2 == 1 ? ASTAR_PARALLEL_COST : ASTAR_DIAGONAL_COST;
}

inline aster_cost_t astar_estimate_cost(point *a, point *b) {
  size_t row_diff = a->row > b->row ? a->row - b->row : b->row - a->row;
  size_t col_diff = a->col > b->col ? a->col - b->col : b->col - a->col;
  return (row_diff + col_diff) * ASTAR_PARALLEL_COST;
}
