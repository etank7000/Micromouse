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
#include "pathfinder.h"
#include "maze.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

static const unsigned int ENC_FLASH_RELOAD = 880UL;
static const unsigned int ENC_MODE_RELOAD = 3520UL;
static const unsigned int NUM_MODES = 8UL;
static const int REC_START = 3200;
static const int LF_TURN = 901; // Threshold for starting curve turn
static const int RF_TURN = 1001;

static int curveTurnCounter = 0;

enum State
{
  IDLE,
  CHOOSING_OPTION,  // User chooses an option for the mouse
  CHOOSING_MODE,   // User is currently choosing the mode for the mouse
  LOCKED,     // User locks in his mode choice (by pressing BOOT0 button)
  RUNNING,    // The mouse is running in one of the 8 operating modes
  CRASH
};

// Determines which mode the mouse operates in while in RUNNING State
static volatile uint32_t g_modeNum = 0;

// Determines what state the mouse is in
static volatile enum State g_state = IDLE;

static volatile uint32_t g_readWriteFlash = 0;

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
  // User has locked in mode choice. Block front sensors to start running.
  if (g_state == LOCKED)
  {
    readReceivers();
    if (getRecLF() > REC_START && getRecRF() > REC_START)
    {
      g_state = IDLE;
    }
  }
  else if (g_state == RUNNING)  // Systick part of mode operation
  {
    switch (g_modeNum)
    {
      case 1:
        readReceivers();
        break;
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
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
        case CHOOSING_OPTION:
          g_state = IDLE;
          break;
        case CHOOSING_MODE:
          g_state = LOCKED;
          break;
        case LOCKED:
          // g_state = CHOOSING_MODE;
          // break;
        case RUNNING:
          g_state = CRASH;
          break;
        case IDLE:
        case CRASH:
          break;
      }
      break;
    /* case INT_Pin: */
    /*   break; */
  }
}

static inline void chooseFlash(void) 
{
  g_readWriteFlash = getLeftEnc() / (ENC_FLASH_RELOAD / 2UL);
  if (g_readWriteFlash & 1) set (LED3); else reset(LED3);
}

// Rotate the left wheel to select among the 8 modes. The LEDs will light
// up to correspond to the mode number. An unlit LED represents a binary 0
// while a lit LED represents a binary 1. The value is read left to right.
static inline void chooseMode(void)
{
  g_modeNum = getLeftEnc() / (ENC_MODE_RELOAD / NUM_MODES);
  if (g_modeNum & 1) set(LED3); else reset(LED3);
  if (g_modeNum & 2) set(LED2); else reset(LED2);
  if (g_modeNum & 4) set(LED1); else reset(LED1);
}

static inline void searchMaze(int doCurveTurn, int doExtraAdjust)
{
  MouseMovement nextMove = getNextMovement();
  switch (nextMove) 
  {
    case MoveForward:
      moveForward(1.0f);
      break;
    case TurnClockwise:
      if (doCurveTurn && curveTurnCounter <= 3) {
        curveTurnCounter++;
        if (frontWallDetected()) {
          while (getRecLF() < LF_TURN && getRecRF() < RF_TURN);
        } else {
          moveForward(0.18f);
        }
        turn(RightTurn, CurveTurn);
        moveForward(0.12f);
      } else {
        curveTurnCounter = 0;
        stopAtCellCenter();
        if (frontWallDetected()) {
          adjust();
        }

        if (!doExtraAdjust) {
          turn(RightTurn, InPlaceTurn);
        } else {
          if (leftWallDetected()) {
            turn(LeftTurn, InPlaceTurn);
            adjust();
            turnAround();
          } else  {
            turn(RightTurn, InPlaceTurn);
          }
        }

        moveForward(0.52f);
      }
      break;
    case TurnCounterClockwise:
      if (doCurveTurn && curveTurnCounter <= 3) {
        curveTurnCounter++;
        if (frontWallDetected()) {
          while (getRecLF() < LF_TURN && getRecRF() < RF_TURN);
        } else {
          moveForward(0.18f);
        }
        turn(LeftTurn, CurveTurn);
        moveForward(0.12f);
      } else {
        curveTurnCounter = 0;
        stopAtCellCenter();
        if (frontWallDetected()) {
          adjust();
        }

        if (!doExtraAdjust) {
          turn(LeftTurn, InPlaceTurn);
        } else {
          if (rightWallDetected()) {
            turn(RightTurn, InPlaceTurn);
            adjust();
            turnAround();
          } else {
            turn(LeftTurn, InPlaceTurn);
          }
        }

        moveForward(0.52f);
      }
      break;
    case TurnAround:
      stopAtCellCenter();
      if (frontWallDetected()) {
        adjust();
      }
      if (rightWallDetected()) {
        turn(RightTurn, InPlaceTurn);
        adjust();
        turn(RightTurn, InPlaceTurn);
      } else if (leftWallDetected()) {
        turn(LeftTurn, InPlaceTurn);
        adjust();
        turn(LeftTurn, InPlaceTurn);
      } else {
        turnAround();
      }
      moveForward(0.52f);
      break;
    case Wait:
      break;
    case Finish:
      stopAtCellCenter();
      g_state = IDLE;
      break;
  }
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

  // initializeMaze();
  // printMaze();
  // saveMazeInFlash();
  // HAL_Delay(1000);
  // readMazeFromFlash();
  // printMaze();
  // while (1) {
  //   // print("stuck in while loop\r\n");
  // }

  int firstTime = 1;
  set(MODE);  // This MODE is the motor driver input
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);

  while (1) {
    __HAL_TIM_SET_AUTORELOAD(&htim2, ENC_FLASH_RELOAD - 1UL);
    resetLeftEnc();
    resetRightEnc();
    g_state = CHOOSING_OPTION;
    while (g_state == CHOOSING_OPTION)
      chooseFlash();
    
    if (g_readWriteFlash)
      readMazeFromFlash();

    HAL_Delay(500);

    // Change the encoder autoreload value to wrap back to 0 when it reaches
    // 3520. This is done for mode selection.
    __HAL_TIM_SET_AUTORELOAD(&htim2, ENC_MODE_RELOAD - 1UL);
    resetLeftEnc();
    resetRightEnc();

    // Use the left wheel to select a mode.
    // Turn left wheel to choose the mode. Press BOOT0 button to lock in choice.
    g_state = CHOOSING_MODE;
    while (g_state == CHOOSING_MODE)
      chooseMode();

    while (g_state == LOCKED);  // Better alternative to polling?

    // Reaching here means we blocked the IR sensors. Prepare for running or 
    // debugging by setting up the motor driver and encoder.
    int i = 0;
    while (i < 6)
    {
      toggle(LED1);
      toggle(LED2);
      toggle(LED3);
      i++;
      HAL_Delay(500);
    }
    reset(LED1);
    reset(LED2);
    reset(LED3);
    __HAL_TIM_SET_AUTORELOAD(&htim2, ULONG_MAX);
    if (firstTime) {
      if (!g_readWriteFlash)
        initializeMaze();
      else
        initializePathFinder();
      firstTime = 0;
    }
    resetMousePosition();
    setFirstCell();
    resetLeftEnc();
    resetRightEnc();
    HAL_Delay(2);
    readReceivers();
    HAL_Delay(2);
    g_state = RUNNING;
    HAL_Delay(1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (g_state == RUNNING)
    {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

      switch (g_modeNum)
      {
        case 0: // Read and print IR sensor values
          printSensorValues();
          break;
        case 1:
          // debugSpeedProfile();
          // g_state = IDLE;

          testAdjust();
          break;
        case 2: // Test Turning
          // turnAround();

          // turn(LeftTurn, InPlaceTurn);
          // HAL_Delay(730);
          turn(RightTurn, InPlaceTurn);
          HAL_Delay(730);

          // moveForward(0.18f);
          // turn(LeftTurn, CurveTurn);
          // moveForward(0.12f);
          break;
        case 3:
          moveUntilWall();
          moveForward(0.55);
          stop();
          adjust();
          turnAround();
          break;
        case 4:
          searchMaze(1, 1);
          break;
        case 5: // Search mode
          searchMaze(1, 0);
          break;
        case 6: // Speed mode 1
          searchMaze(0, 0);
          break;
        case 7: // Speed mode 2
          searchMaze(0, 1);
          break;
      }
    }
    resetSpeedProfile();
    if (g_state != CRASH) {
      saveMazeInFlash();
      HAL_Delay(100);
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
