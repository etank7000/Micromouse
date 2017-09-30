#include "encoder.h"
#include "tim.h"

uint32_t getLeftEncCount(void)
{
  return __HAL_TIM_GET_COUNTER(&htim2);
}

uint32_t getRightEncCount(void)
{
  return __HAL_TIM_GET_COUNTER(&htim5);
}

void resetLeftEncCount(void)
{
  __HAL_TIM_SET_COUNTER(&htim2, 0);
}

void resetRightEncCount(void)
{
  __HAL_TIM_SET_COUNTER(&htim5, 0);
}
