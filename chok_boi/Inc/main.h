/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/**
 * @file    main.h
 * @brief   This file contains the common defines of the application
 */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define ENC_RA_Pin GPIO_PIN_0
#define ENC_RA_GPIO_Port GPIOA
#define ENC_RB_Pin GPIO_PIN_1
#define ENC_RB_GPIO_Port GPIOA
#define EM_RF_Pin GPIO_PIN_2
#define EM_RF_GPIO_Port GPIOA
#define VOLTMETER_Pin GPIO_PIN_3
#define VOLTMETER_GPIO_Port GPIOA
#define REC_RF_Pin GPIO_PIN_4
#define REC_RF_GPIO_Port GPIOA
#define REC_RH_Pin GPIO_PIN_5
#define REC_RH_GPIO_Port GPIOA
#define EM_H_Pin GPIO_PIN_6
#define EM_H_GPIO_Port GPIOA
#define EM_D_Pin GPIO_PIN_7
#define EM_D_GPIO_Port GPIOA
#define REC_RD_Pin GPIO_PIN_4
#define REC_RD_GPIO_Port GPIOC
#define REC_LD_Pin GPIO_PIN_5
#define REC_LD_GPIO_Port GPIOC
#define REC_LH_Pin GPIO_PIN_0
#define REC_LH_GPIO_Port GPIOB
#define REC_LF_Pin GPIO_PIN_1
#define REC_LF_GPIO_Port GPIOB
#define INT_Pin GPIO_PIN_10
#define INT_GPIO_Port GPIOB
#define SS1_Pin GPIO_PIN_7
#define SS1_GPIO_Port GPIOC
#define EM_LF_Pin GPIO_PIN_11
#define EM_LF_GPIO_Port GPIOA
#define ENC_LA_Pin GPIO_PIN_15
#define ENC_LA_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_10
#define LED1_GPIO_Port GPIOC
#define LED2_Pin GPIO_PIN_11
#define LED2_GPIO_Port GPIOC
#define LED3_Pin GPIO_PIN_12
#define LED3_GPIO_Port GPIOC
#define ENC_LB_Pin GPIO_PIN_3
#define ENC_LB_GPIO_Port GPIOB
#define MODE_Pin GPIO_PIN_4
#define MODE_GPIO_Port GPIOB
#define BOOT0_Pin GPIO_PIN_5
#define BOOT0_GPIO_Port GPIOB
#define BOOT0_EXTI_IRQn EXTI9_5_IRQn
#define PH_L_Pin GPIO_PIN_7
#define PH_L_GPIO_Port GPIOB
#define PH_R_Pin GPIO_PIN_9
#define PH_R_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
