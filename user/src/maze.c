#include "maze.h"
#include "bit_vector_256.h"
#include "dir.h"
#include "pathfinder.h"
#include "ir_sensor.h"

static BitVector256 wallsNS = {{0}};
static BitVector256 wallsEW = {{0}};
static Dir heading = NORTH;
static unsigned short mouseX = 0;
static unsigned short mouseY = 0;

/**
 * @brief Place a wall in direction d of the cell at location (x, y)
 * 
 * @retval None
 */ 
static void setClosed(unsigned short x, unsigned short y, Dir d) {
  switch (d) {
    case NORTH:
      return bitvector_clear(wallsNS, x, y+1);
    case SOUTH:
      return bitvector_clear(wallsNS, x, y);
    case EAST:
      return bitvector_clear(wallsEW, x+1, y);
    case WEST:
      return bitvector_clear(wallsEW, x, y);
    case INVALID:
    default:
      return;
  }
}

static void moveForward(void) {
  if (!isOpen(mouseX, mouseY, heading))
    return;

  switch (heading) {
    case NORTH:
      mouseY++;
      break;
    case SOUTH:
      mouseY--;
      break;
    case EAST:
      mouseX++;
      break;
    case WEST:
      mouseX--;
      break;
    case INVALID:
    default:
      break;
  }
}

static inline void turnClockwise(void) {
  heading = clockwise(heading);
}

static inline void turnCounterClockwise(void) {
  heading = counterClockwise(heading);
}

static inline void turnAround(void) {
  heading = opposite(heading);
}

static inline void updateWalls(void) {
  if (frontWallDetected())
    setClosed(mouseX, mouseY, heading);
  if (leftWallDetected())
    setClosed(mouseX, mouseY, counterClockwise(heading));
  if (rightWallDetected())
    setClosed(mouseX, mouseY, clockwise(heading));
}

void initializeMaze(void) {
  // Assume no walls
  bitvector_setAll(wallsNS);
  bitvector_setAll(wallsEW);

  // Close the borders of the maze
  for (int i = 0; i < MAZE_LEN; i++) {
		setClosed(i, 0, SOUTH);
		setClosed(0, i, WEST);
		setClosed(i, MAZE_LEN - 1, NORTH);
		setClosed(MAZE_LEN - 1, i, EAST);
  }

  initializePathFinder();
}

int isOpen(unsigned short x, unsigned short y, Dir d) {
  switch (d) {
    case NORTH:
      return bitvector_get(wallsNS, x, y+1);
    case SOUTH:
      return bitvector_get(wallsNS, x, y);
    case EAST:
      return bitvector_get(wallsEW, x+1, y);
    case WEST:
      return bitvector_get(wallsEW, x, y);
    case INVALID:
    default:
      return 0;
  }
}

int wallInFront(void) {
  return !isOpen(mouseX, mouseY, heading);
}

int wallOnLeft(void) {
  return !isOpen(mouseX, mouseY, counterClockwise(heading));
}

int wallOnRight(void) {
  return !isOpen(mouseX, mouseY, clockwise(heading));
}

MouseMovement getNextMovement(void) {
  updateWalls();
  MouseMovement nextMove = nextMovement(mouseX, mouseY, heading);
  switch (nextMove) {
    case MoveForward:
      moveForward();
      break;
    case TurnClockwise:
      turnClockwise();
      moveForward();
      break;
    case TurnCounterClockwise:
      turnCounterClockwise();
      moveForward();
      break;
    case TurnAround:
      turnAround();
      moveForward();
      break;
    case Wait:
      break;
    case Finish:
    default:
      break;
  }
  return nextMove;
}
