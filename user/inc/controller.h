/**
 * @file    controller.h
 * @brief   Header file for the controller.
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "stm32f4xx_hal.h"

/**
 * @brief Controls mouse movement.
 */
void speedProfile(void);

/**
 * @brief Reset all speed profile variables.
 */
void resetSpeedProfile(void);

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

void turn(void);

/**
 * @brief Turn right 90 degrees. Use for curve turn.
 */
void turnRight(void);

/**
 * @brief Turn left 90 degrees. Use for curve turn.
 */
void turnLeft(void);

/**
 * @brief Turn right 90 degrees. Use for still turn.
 */
void turnRightStill(void);

/**
 * @brief Turn left 90 degrees. Use for still turn.
 */
void turnLeftStill(void);

/**
 * @brief Turn 180 degrees.
 */
void turnAround(void);

/**
 * @brief Stop the mouse at the center of the cell.
 * 
 * @retval  None
 */
void stop(void);

/**
 * @brief Adjust the front sensors to a front wall when mouse stops.
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
