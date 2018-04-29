/**
 * @file    ir_sensor.h
 * @brief   The header file for reading the IR sensors.
 */

#ifndef IR_SENSOR_H
#define IR_SENSOR_H

#include "stm32f4xx_hal.h"
#include "main.h"
#include "adc.h"

/**
 * @brief Take a reading of the left forward sensor.
 */
#define readRecLF() readADC(&hadc1, ADC_CHANNEL_9, ADC_SAMPLETIME_84CYCLES)

/**
 * @brief Take a reading of the right forward sensor.
 */
#define readRecRF() readADC(&hadc1, ADC_CHANNEL_4, ADC_SAMPLETIME_84CYCLES)

/**
 * @brief Take a reading of the left horizontal sensor.
 */
#define readRecLH() readADC(&hadc1, ADC_CHANNEL_8, ADC_SAMPLETIME_84CYCLES)

/**
 * @brief Take a reading of the right horizontal sensor.
 */
#define readRecRH() readADC(&hadc1, ADC_CHANNEL_5, ADC_SAMPLETIME_84CYCLES)

/**
 * @brief Take a reading of the left diagonal sensor.
 */
#define readRecLD() readADC(&hadc1, ADC_CHANNEL_15, ADC_SAMPLETIME_84CYCLES)

/**
 * @brief Take a reading of the right diagonal sensor.
 */
#define readRecRD() readADC(&hadc1, ADC_CHANNEL_14, ADC_SAMPLETIME_84CYCLES)

void readReceivers(void);

/**
 * @brief Get the most recent reading of the left forward sensor.
 * @return The most recent reading of the left forward sensor.
 */
int32_t getRecLF();

/**
 * @brief Get the most recent reading of the right forward sensor.
 * @return The most recent reading of the right forward sensor.
 */
int32_t getRecRF();

/**
 * @brief Get the most recent reading of the left horizontal sensor.
 * @return The most recent reading of the left horizontal sensor.
 */
int32_t getRecLH();

/**
 * @brief Get the most recent reading of the right horizontal sensor.
 * @return The most recent reading of the right horizontal sensor.
 */
int32_t getRecRH();

/**
 * @brief Get the most recent reading of the left diagonal sensor.
 * @return The most recent reading of the left diagonal sensor.
 */
int32_t getRecLD();

/**
 * @brief Get the most recent reading of the right diagonal sensor.
 * @return The most recent reading of the right diagonal sensor.
 */
int32_t getRecRD();

/**
 * @brief Determine if there is a wall in front of the mouse
 * @return 1 if there is a wall in front of the mouse, 0 otherwise
 */
int32_t frontWallDetected(void);

/**
 * @brief Determine if there is a wall to the left of the mouse
 * @return 1 if there is a wall to the left of the mouse, 0 otherwise
 */
int32_t leftWallDetected(void);

/**
 * @brief Determine if there is a wall to the right of the mouse
 * @return 1 if there is a wall to the right of the mouse, 0 otherwise
 */
int32_t rightWallDetected(void);

#endif  // IR_SENSOR_H
