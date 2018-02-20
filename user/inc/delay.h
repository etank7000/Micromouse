/**
 * @file    delay.h
 * @brief   Timer functions.
 */

#ifndef DELAY_H
#define DELAY_H

#include "stm32f4xx_hal.h"

/**
 * @brief System clock time in microseconds.
 * @return The number of microseconds since the start of the program.
 */
uint32_t micros();

#endif  // DELAY_H
