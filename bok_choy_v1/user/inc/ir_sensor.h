#ifndef IR_SENSOR_H
#define IR_SENSOR_H

#include "stm32f4xx_hal.h"
#include "main.h"
#include "adc.h"

#define readRecLF() readADC(&hadc1, ADC_CHANNEL_15, ADC_SAMPLETIME_84CYCLES);
#define readRecRF() readADC(&hadc1, ADC_CHANNEL_4, ADC_SAMPLETIME_84CYCLES);
#define readRecLS() readADC(&hadc1, ADC_CHANNEL_14, ADC_SAMPLETIME_84CYCLES);
#define readRecRS() readADC(&hadc1, ADC_CHANNEL_6, ADC_SAMPLETIME_84CYCLES);

void readReceivers(void);

int32_t getRecLF();
int32_t getRecRF();
int32_t getRecLS();
int32_t getRecRS();

#endif  // IR_SENSOR_H
