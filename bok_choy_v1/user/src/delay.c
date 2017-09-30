#include "delay.h"

uint32_t micros()
{
  return HAL_GetTick()*1000U + 1000U - SysTick->VAL / (HAL_RCC_GetHCLKFreq() / 1000000U); 
}
