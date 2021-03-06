/**
 * @file    encoder.h
 * @brief   Header file for the encoders.
 */

#ifndef ENCODER_H
#define ENCODER_H

#include "stm32f4xx_hal.h"

/**
 * @brief Left encoder step count.
 * @return The left encoder reading.
 */
uint32_t getLeftEnc(void);

/**
 * @brief Right encoder step count.
 * @return The right encoder reading.
 */
uint32_t getRightEnc(void);

/**
 * @brief Reset the left encoder step count.
 *
 * This function sets the left encoder step count to 0.
 */
void resetLeftEnc(void);

/**
 * @brief Reset the right encoder step count.
 *
 * This function sets the right encoder step count to 0.
 */
void resetRightEnc(void);

#endif  // ENCODER_H
