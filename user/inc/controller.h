/**
 * @file    controller.h
 * @brief   Header file for the controller.
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "stm32f4xx_hal.h"

typedef enum TurnDir {
  LeftTurn,
  RightTurn
} TurnDir;

typedef enum TurnMotion {
  CurveTurn,
  InPlaceTurn
} TurnMotion;

/**
 * @brief Controls mouse movement.
 */
void speedProfile(void);

/**
 * @brief Reset all speed profile variables.
 */
void resetSpeedProfile(void);

void setFirstCell(void);

/**
 * @brief Move straight until the mouse sees a front wall.
 */
void moveUntilWall(void);

/**
 * @brief Move forward nCells cell.
 * 
 * @param nCells the number of cells to move forward.
 * 
 * @retval  None
 */
void moveForward(float nCells);

/**
 * @brief Turn either left or right 90 degrees.
 * 
 * @param turnDirection the direction to turn, either LeftTurn or RightTurn.
 * @param turnMotion  the motion the mouse turns from, either CurveTurn 
 *                    or InPlaceTurn.
 * 
 * @retval  None
 */
void turn(TurnDir turnDirection, TurnMotion turnMotion);

/**
 * @brief Turn 180 degrees.
 */
void turnAround(void);

/**
 * @brief Stop the mouse by decelerating to zero speed.
 * 
 * @retval  None
 */
void stop(void);

/**
 * @brief Stop the mouse at the center of the cell.
 * 
 * @retval  None
 */
void stopAtCellCenter(void);

/**
 * @brief Adjust the front sensors to a wall in front.
 * 
 * @retval  None
 */
void adjust(void);

void testAdjust(void);

/**
 * @brief Get current translational speed according to encoder feedback.
 */
float getEncSpeedX(void);

/**
 * @brief Get ideal current translational speed.
 */
float getCurSpeedX(void);

#endif  // CONTROLLER_H
