/**
 * @file    motor.h
 * @brief   This file contains the headers for operating the motor driver.
 */

#ifndef MOTOR_H
#define MOTOR_H

#include "stm32f4xx_hal.h"
#include "main.h"

/**
 * @brief Set the duty cycle for the left motor.
 *
 * The argument takes in an integer ranging from -999 to 999, where 0 is
 * effectively a 0% duty cycle and 999 is effectively a 100% duty cycle.
 * A positive value will spin the motor forward, and a negative value will
 * spin the motor backward.
 *
 * @param val An integer ranging from -999 to 999.
 */
void setLeftMotor(int32_t val);

/**
 * @brief Set the duty cycle for the right motor.
 *
 * The argument takes in an integer ranging from -999 to 999, where 0 is
 * effectively a 0% duty cycle and 999 is effectively a 100% duty cycle.
 * A positive value will spin the motor forward, and a negative value will
 * spin the motor backward.
 *
 * @param val An integer ranging from -999 to 999.
 */
void setRightMotor(int32_t val);

#endif  // MOTOR_H
