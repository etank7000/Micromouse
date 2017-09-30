#ifndef ENCODER_H
#define ENCODER_H

#include "stm32f4xx_hal.h"

uint32_t getLeftEncCount(void);
uint32_t getRightEncCount(void);

void resetLeftEncCount(void);
void resetRightEncCount(void);

#endif  // ENCODER_H
