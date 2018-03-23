/**
 * @file    battery_checker.h
 * @brief   The header file for reading the battery voltage level.
 */

#ifndef BATTERY_CHECKER_H
#define BATTERY_CHECKER_H

#include "stm32f4xx_hal.h"
#include "main.h"
#include "adc.h"

/**
 * @brief Take a reading of the battery voltage level
 */
#define readRecLF() readADC(&hadc1, ADC_CHANNEL_9, ADC_SAMPLETIME_84CYCLES)

int32_t getVoltage(void);

void checkBattery(void);

#endif  // BATTERY_CHECKER_H
