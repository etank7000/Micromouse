#include "pathfinder.h"

#if defined PATHFINDER && PATHFINDER == STRAIGHTPATHFOLLOWER

#include "dir.h"
#include "maze.h"

void initializePathFinder(void) {}

MouseMovement nextMovement(unsigned x, unsigned y, Dir heading) {
  if (!wallInFront()) {
    return MoveForward;
  } else if (!wallOnLeft()) {
    return TurnCounterClockwise;
  } else if (!wallOnRight()) {
    return TurnClockwise;
  } else {
    return TurnAround;
  }
}

#endif
