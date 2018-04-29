#ifndef MAZE_H
#define MAZE_H

#include "bit_vector_256.h"
#include "dir.h"
#include "pathfinder.h"

#define MAZE_LEN 16

/**
 * @brief Initialize the wall placement of the maze and initializes the pathfinder.
 * 
 * @retval  None
 */
void initializeMaze(void);

/**
 * @brief Determine if there is no wall in direction d of Cell (x, y).
 * 
 * @retval  1 if there is no wall, 0 if there is a wall.
 */
int isOpen(unsigned short x, unsigned short y, Dir d);

/**
 * @brief Determine whether there is a wall in front of the mouse's
 *        current location.
 * 
 * @retval  1 if there is a wall, 0 if there is no such wall.
 */
int wallInFront(void);

/**
 * @brief Determine whether there is a wall to the left of the mouse's
 *        current location.
 * 
 * @retval  1 if there is a wall, 0 if there is no such wall.
 */
int wallOnLeft(void);

/**
 * @brief Determine whether there is a wall to the right of the mouse's
 *        current location.
 * 
 * @retval  1 if there is a wall, 0 if there is no such wall.
 */
int wallOnRight(void);

/**
 * @brief Determine the next movement of the mouse and update the mouse's
 *        (x, y) location and heading direction based on the movement.
 * 
 * @retval  A MouseMovement representing the mouse's next move.
 */
MouseMovement getNextMovement(void);

#endif  // MAZE_H
