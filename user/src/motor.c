#include "motor.h"
#include "tim.h"
#include "gpio.h"

#define PERIOD (int32_t)__HAL_TIM_GET_AUTORELOAD(&htim4)

void setLeftMotor(int32_t val)
{
  if (val > PERIOD)
  {
    val = PERIOD;
  }
  else if (val < -PERIOD)
  {
    val = -PERIOD;
  }

  if (val > 0)
  {
    reset(PH_L);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, val);
  }
  else
  {
    set(PH_L);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, -val);
  }
}

void setRightMotor(int32_t val)
{
  if (val > PERIOD)
  {
    val = PERIOD;
  }
  else if (val < -PERIOD)
  {
    val = -PERIOD;
  }

  if (val > 0)
  {
    set(PH_R);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, val);
  }
  else
  {
    reset(PH_R);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, -val);
  }
}
