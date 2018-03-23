#include "battery_checker.h"

int32_t getVoltage(void)
{
  int32_t raw = readADC(&hadc1, ADC_CHANNEL_3, ADC_SAMPLETIME_84CYCLES);
  // returns 1000 times the actual voltage for printing purposes
  return raw * 1000 / 4096 * 3.3; 
}

void checkBattery(void)
{
}
