#ifndef PATHFINDER_H
#define PATHFINDER_H

// Options for which pathfinding algorithm to use
#define FLOODFILL 1
#define STRAIGHTPATHFOLLOWER 2
#define LEFTWALLFOLLOWER 3
#define RIGHTWALLFOLLOWER 4

// Change this value to choose the pathfinding algorithm
#define PATHFINDER FLOODFILL

#include "dir.h"

typedef enum MouseMovement {
    MoveForward,            // Move in the direction mouse is facing
    TurnClockwise,          // Self explanatory
    TurnCounterClockwise,   // Self explanatory
    TurnAround,             // Face the opposite direction currently facing
    Wait,                   // Do some computation, then try again later
    Finish                  // Mouse has achieved goals
} MouseMovement;

/**
 * @brief   Lets the PathFinder perform any initialization it needs
 * 
 * @retval  None
 */
void initializePathFinder(void);

/**
 * @brief   Calculates what the next move for the mouse should be based on
 *          walls of the maze.
 * 
 * @param x The mouse's x-coordinate
 * @param y The mouse's y-coordinate
 * @param heading The mouse's current direction.
 * 
 * @retval  The MouseMovement of the mouse's next move.
 */
MouseMovement nextMovement(unsigned short x, unsigned short y, Dir heading);

#endif  // PATHFINDER_H