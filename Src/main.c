/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include <limits.h>

#include "ir_sensor.h"
#include "encoder.h"
#include "controller.h"
#include "debug.h"
#include "motor.h"
#include "gyro.h"
#include "battery_checker.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

static const unsigned int ENC_MODE_RELOAD = 3520UL;
static const unsigned int NUM_MODES = 8UL;
static const int REC_START = 3200;

enum State
{
  IDLE,
  CHOOSING,
  LOCKED,
  RUNNING
};

static volatile uint32_t g_modeNum = 0;
static volatile enum State g_state = IDLE;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

void HAL_SYSTICK_Callback(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
static void chooseMode(void);

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

// This is the 1ms systick interrupt.
void HAL_SYSTICK_Callback(void)
{
  if (g_state == LOCKED)
  {
    readReceivers();
    if (getRecLF() > REC_START && getRecRF() > REC_START)
    {
      g_state = IDLE;
    }
  }
  else if (g_state == RUNNING)
  {
    switch (g_modeNum)
    {
      case 2:
        readReceivers();
        speedProfile();
        updateSpeedData();
        break;
      case 5:
        readReceivers();
        speedProfile();
        break;
    }
  }
}

// This function gets called when the BOOT0 button is pressed
// or when the INT signal from the gyro is received.
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch (GPIO_Pin)
  {
    case BOOT0_Pin:
      switch (g_state)
      {
        case CHOOSING:
          g_state = LOCKED;
          break;
        case LOCKED:
          g_state = CHOOSING;
          break;
        case RUNNING:
        case IDLE:
          break;
      }
      break;
    /* case INT_Pin: */
    /*   break; */
  }
}

// Rotate the left wheel to select among the 8 modes. The LEDs will light
// up to correspond to the mode number. An unlit LED represents a binary 0
// while a lit LED represents a binary 1. The value is read left to right.
static void chooseMode(void)
{
  g_modeNum = getLeftEnc() / (ENC_MODE_RELOAD / NUM_MODES);
  if (g_modeNum & 1) set(LED3); else reset(LED3);
  if (g_modeNum & 2) set(LED2); else reset(LED2);
  if (g_modeNum & 4) set(LED1); else reset(LED1);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_SPI2_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  // print("%hd\r\n", who_am_i());
  // gyro_spi_init();
  // HAL_Delay(100); 
  // set_gyro_scale();
  // HAL_Delay(1000); 
  // calibrateGyro();
  // HAL_Delay(1000);

  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);

  // This loop is for the initial test to make sure all the sensors have
  // proper readings. Comment out or remove after verification.
  /* while (1) */
  /* { */
  /*   printSensorValues(); */
  /*   print("%d\r\n", getVoltage()); */
  /*   HAL_Delay(100); */
  /* } */

  // Change the encoder autoreload value to wrap back to 0 when it reaches
  // 3520. This is done for mode selection.
  __HAL_TIM_SET_AUTORELOAD(&htim2, ENC_MODE_RELOAD - 1UL);

  // Use the left wheel to select a mode.
  g_state = CHOOSING;
  while (g_state == CHOOSING)
  {
    resetLeftEnc();
    resetRightEnc();
    // Turn left wheel to choose the mode. Press BOOT0 button to lock in choice.
    do
    {
      chooseMode();
    } while (g_state == CHOOSING);

    // Can either press BOOT0 button again to choose another mode, or block the 
    // left and right forward IR sensors to start running in desired mode.
    while (g_state == LOCKED);  // Better alternative to polling?

    // If g_state == IDLE then that means we blocked the IR sensors instead
    // of pressing BOOT0 again. Prepare for running or debugging by
    // setting up the motor driver and encoder.
    if (g_state == IDLE)
    {
      int i = 0;
      while (i < 6)
      {
        toggle(LED1);
        toggle(LED2);
        toggle(LED3);
        i++;
        HAL_Delay(500);
      }
      __HAL_TIM_SET_AUTORELOAD(&htim2, ULONG_MAX);
      set(MODE);  // This MODE is the motor driver input
      HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
      HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
      resetLeftEnc();
      resetRightEnc();
      g_state = RUNNING;
    }
  }


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (g_state == RUNNING)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

    switch (g_modeNum)
    {
      case 0:
        printSensorValues();
        break;
      case 1:
        printGyroValues();
        break;
      case 2:
        debugSpeedProfile();
        g_state = IDLE;
        break;
      case 3:
        turnRight();
        HAL_Delay(1500);
        turnLeft();
        HAL_Delay(1500);
        break;
      case 5:
        moveUntilWall();
        turnRight();
        // Search mode
        break;
      case 6:
        //  Speed mode 1
        break;
      case 7:
        // Speed mode 2
        break;
    }
  }

  // Reaching this point means we are done with the main routine. Proceed to
  // enter a low power state.
  while (1)
  {
    resetSpeedProfile();
    HAL_PWR_EnterSTANDBYMode();
  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  g_state = IDLE;
  resetSpeedProfile();
  print("%s: line %d\r\n", file, line);
  while(1) 
  {
    toggle(LED1);
    HAL_Delay(1000);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
