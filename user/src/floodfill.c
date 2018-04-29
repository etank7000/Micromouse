#include "pathfinder.h"

#if defined PATHFINDER && PATHFINDER == FLOODFILL

#include "stack.h"
#include "dir.h"
#include "maze.h"

static const unsigned short INFINITE = 256;

static unsigned short distances[MAZE_LEN][MAZE_LEN] = {0};

/**
 * @brief Initialize the manhattan distance of each cell from the
 *        finishing point cells.
 * 
 * @retval  None
 */
static void initializeDistances(void) {
  // Distance initialization assumes middle 4 squares are finish point
  for (unsigned short y = 0; y < MAZE_LEN / 2; y++) {
    for (unsigned short x = 0; x < MAZE_LEN / 2; x++) {
      distances[y][x] = 
      distances[y][MAZE_LEN - x - 1] =
      distances[MAZE_LEN - y - 1][x] =
      distances[MAZE_LEN - y - 1][MAZE_LEN - x - 1] =
      MAZE_LEN - 2 - y - x;
    }
  }
}

/**
 * @brief Set the distance of the adjacent cell 
 * in direction d of the cell at (x, y).
 * 
 * @param x The x-coordinate of the cell.
 * @param y The y-coordinate of the cell.
 * @param d The direction relative to the cell. Use INVALID to set
 *          the distance of the cell at (x, y).
 * 
 * @retval  None
 */
static void setDistance(unsigned short x, unsigned short y, unsigned short val) {
  if (x >= MAZE_LEN || y >= MAZE_LEN)
    return;
  distances[y][x] = val;
}

/**
 * @brief Get the distance of the adjacent cell 
 * in direction d of the cell at (x, y).
 * 
 * @param x The x-coordinate of the cell.
 * @param y The y-coordinate of the cell.
 * @param d The direction relative to the cell. Use INVALID to get
 *          the distance of the cell at (x, y).
 * 
 * @retval  The distance value of the adjacent cell.
 */
static unsigned short getDistance(unsigned short x, unsigned short y, Dir d) {
  if (x >= MAZE_LEN || y >= MAZE_LEN)
    return INFINITE;
	switch (d) {
		case NORTH:
			return distances[y + 1][x];
		case SOUTH:
			return distances[y - 1][x];
		case EAST:
			return distances[y][x + 1];
		case WEST:
			return distances[y][x - 1];
		case INVALID:
			return distances[y][x];
		default:
			return INFINITE;
	}
}

/**
 * @brief Find the minimum distance of the cells adjacent to the one at (x, y).
 * 
 * @param x The x-coordinate of the cell.
 * @param y The y-coordinate of the cell.
 * 
 * @retval  The minimum distance value of the neighboring cells.
 */
static unsigned short findMinDistanceOfNeighbors(unsigned short x, unsigned short y) {
  unsigned short minDist = INFINITE;
  unsigned short northDist = getDistance(x, y, NORTH);
  unsigned short eastDist = getDistance(x, y, EAST);
  unsigned short southDist = getDistance(x, y,SOUTH);
  unsigned short westDist = getDistance(x, y, WEST);
  if (isOpen(x, y, NORTH) && northDist < minDist)
    minDist = northDist;
  if (isOpen(x, y, EAST) && eastDist < minDist)
    minDist = eastDist;
  if (isOpen(x, y, SOUTH) && southDist < minDist)
    minDist = southDist;
  if (isOpen(x, y, WEST) && westDist < minDist)
    minDist = westDist;
  return minDist;
}

static int atCenter(unsigned short x, unsigned short y) {
  unsigned midpoint = MAZE_LEN / 2;

  return (x == midpoint && y == midpoint) ||
         (x == midpoint - 1 && y == midpoint) ||
         (x == midpoint && y == midpoint - 1) ||
         (x == midpoint - 1 && y == midpoint - 1);
}

static void runFloodFill(unsigned short x, unsigned short y) {
  stack_push(x, y);
	if (!isOpen(x, y, NORTH))
    stack_push(x, y + 1);
	if (!isOpen(x, y, EAST))
		stack_push(x + 1, y);
	if (!isOpen(x, y, SOUTH))
		stack_push(x, y - 1);
	if (!isOpen(x, y, WEST))
		stack_push(x - 1, y);
  
  while (!stack_empty()) {
    Cell cur = stack_top();
    stack_pop();
    if (getDistance(cur.x, cur.y, INVALID) == 0)
      continue;
    unsigned short curDist = getDistance(cur.x, cur.y, INVALID);
    unsigned short minDist = findMinDistanceOfNeighbors(cur.x, cur.y);
    if (curDist == minDist + 1 || minDist >= INFINITE)
      continue;
    setDistance(cur.x, cur.y, minDist + 1);
    if (isOpen(x, y, NORTH))
      stack_push(x, y + 1);
    if (isOpen(x, y, EAST))
      stack_push(x + 1, y);
    if (isOpen(x, y, SOUTH))
      stack_push(x, y - 1);
    if (isOpen(x, y, WEST))
      stack_push(x - 1, y);
  }
}

void initializePathFinder(void) {
  initializeDistances();
}

MouseMovement nextMovement(unsigned short x, unsigned short y, Dir heading) {
  if (atCenter(x, y)) {
    return Finish;
  }
  runFloodFill(x, y);
  unsigned short curDist = getDistance(x, y, INVALID);
  if (!wallInFront() && getDistance(x, y, heading) < curDist) {
    return MoveForward;
  } else if (!wallOnLeft() && getDistance(x, y, counterClockwise(heading)) < curDist) {
    return TurnCounterClockwise;
  } else if (!wallOnRight() && getDistance(x, y, clockwise(heading)) < curDist) {
    return TurnClockwise;
  } else if (getDistance(x, y, opposite(heading)) < curDist) {
    return TurnAround;
  }
  return Wait;
}

#endif
