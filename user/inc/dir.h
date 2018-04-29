#ifndef DIR_H
#define DIR_H

typedef enum Dir {
  NORTH = 0,
  SOUTH,
  EAST,
  WEST,
  INVALID
} Dir;

inline Dir opposite(Dir d) {
  switch (d) {
    case NORTH:
      return SOUTH;
    case SOUTH:
      return NORTH;
    case EAST:
      return WEST;
    case WEST:
      return EAST;
    case INVALID:
    default:
      return INVALID;
  }
}

inline Dir clockwise(Dir d) {
  switch (d) {
    case NORTH:
      return EAST;
    case SOUTH:
      return WEST;
    case EAST:
      return SOUTH;
    case WEST:
      return NORTH;
    case INVALID:
    default:
      return INVALID;
  }
}

inline Dir counterClockwise(Dir d) {
  switch (d) {
    case NORTH:
      return WEST;
    case SOUTH:
      return EAST;
    case EAST:
      return NORTH;
    case WEST:
      return SOUTH;
    case INVALID:
    default:
      return INVALID;
  }
}

#endif  // DIR_H
