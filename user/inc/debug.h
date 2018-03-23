/**
 * @file    debug.h
 * @brief   Header file for debugging modes.
 */

#ifndef DEBUG_H
#define DEBUG_H

#include "stm32f4xx_hal.h"

/**
 * @brief Print readings from IR sensors and encoders.
 */
void printSensorValues(void);

/**
 * @brief Print readings from gyro.
 */
void printGyroValues(void);

/**
 * @brief Update data for plotting speed profile.
 */
void updateSpeedData(void);

/**
 * @brief Move forward with motors and encoder feedback.
 *
 * The mouse should decelerate to a stop when its forward IR sensors detect
 * a wall close enough.
 */
void debugSpeedProfile(void);

void turnInPlace(void);

#endif  // DEBUG_H
