#include "encoder.h"
#include "tim.h"

uint32_t getLeftEnc(void)
{
  return __HAL_TIM_GET_COUNTER(&htim2);
}

uint32_t getRightEnc(void)
{
  return __HAL_TIM_GET_COUNTER(&htim5);
}

void resetLeftEnc(void)
{
  __HAL_TIM_SET_COUNTER(&htim2, 0);
}

void resetRightEnc(void)
{
  __HAL_TIM_SET_COUNTER(&htim5, 0);
}
