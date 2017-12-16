#ifndef IR_SENSOR_H
#define IR_SENSOR_H

#include "stm32f4xx_hal.h"
#include "main.h"
#include "adc.h"

#define readRecLF() readADC(&hadc1, ADC_CHANNEL_9, ADC_SAMPLETIME_84CYCLES);
#define readRecRF() readADC(&hadc1, ADC_CHANNEL_4, ADC_SAMPLETIME_84CYCLES);
#define readRecLH() readADC(&hadc1, ADC_CHANNEL_8, ADC_SAMPLETIME_84CYCLES);
#define readRecRH() readADC(&hadc1, ADC_CHANNEL_5, ADC_SAMPLETIME_84CYCLES);
#define readRecLD() readADC(&hadc1, ADC_CHANNEL_15, ADC_SAMPLETIME_84CYCLES);
#define readRecRD() readADC(&hadc1, ADC_CHANNEL_14, ADC_SAMPLETIME_84CYCLES);

void readReceivers(void);

int32_t getRecLF();
int32_t getRecRF();
int32_t getRecLH();
int32_t getRecRH();
int32_t getRecLD();
int32_t getRecRD();

#endif  // IR_SENSOR_H
