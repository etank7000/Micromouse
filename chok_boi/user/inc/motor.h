#ifndef MOTOR_H
#define MOTOR_H

#include "stm32f4xx_hal.h"
#include "main.h"

void setLeftMotor(int32_t val);
void setRightMotor(int32_t val);

#endif  // MOTOR_H
