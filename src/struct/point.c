#include "struct/point.h"
#include "util/debug.h"

const direction_t directions[DIRECTION_LENGTH] = {
    DIRECTION_EAST,       DIRECTION_SOUTH,      DIRECTION_WEST,
    DIRECTION_NORTH,      DIRECTION_NORTH_EAST, DIRECTION_SOUTH_EAST,
    DIRECTION_SOUTH_WEST, DIRECTION_NORTH_WEST,
};

inline direction_t *direction_start() { return (direction_t *)directions; }
inline direction_t *direction_end() {
  return direction_start() + DIRECTION_LENGTH;
}
direction_t *direction_next(direction_t *ptr) {
  if (ptr >= direction_start() && ptr < direction_end()) {
    return ptr + 1;
  }
  return direction_end();
}

char *direction_str(direction_t d) {
  static char *strs[DIRECTION_SOUTH_EAST + 1] = {"↖", "↑", "↗", "←", " ",
                                                 "→", "↙", "↓", "↘"};
  return strs[(unsigned)d % (DIRECTION_SOUTH_EAST + 1)];
}

inline direction_t direction_reverse(const direction_t direction) {
  // debugf("\t\tdirection_reverse %s to %s\n", direction_str(direction),
  //        direction_str(DIRECTION_SOUTH_EAST - direction));
  return DIRECTION_SOUTH_EAST - direction;
}

inline point point_move(const point from, const direction_t direction) {
  // debugf("\tpoint (%zu, %zu) move %s\n", from.row, from.col,
  //        direction_str(direction));
  point to = from;
  to.row += direction / 3 - 1;
  to.col += direction % 3 - 1;
  return to;
}

inline bool point_equal(const point from, const point to) {
  return from.row == to.row && from.col == to.col;
}

inline direction_t point_move_direction(const point from, const point to) {
  direction_t row = (direction_t)(to.row - from.row + 1);
  direction_t col = (direction_t)(to.col - from.col + 1);
  return row * 3 + col;
}
