#include "gyro.h"
#include "gpio.h"
#include "spi.h"
#include "usart.h"

void gyro_spi_init(void) {
  uint8_t pAddr;
  uint8_t pData;
  uint8_t buf[2];

  // Disable I2C
  buf[0] = MPUREG_USER_CTRL;
  buf[1] = BIT_I2C_IF_DIS;
  reset(SS1);
  HAL_SPI_Transmit(&hspi2, buf, sizeof(buf), 100);
  set(SS1);
  HAL_Delay(10);
  /* print("WHOAMI: %hd\r\n", who_am_i()); */

  // Test print value of register.
  /* pAddr = MPUREG_USER_CTRL | READ_FLAG; */
  /* /1* pData = 0; *1/ */
  /* reset(SS1); */
  /* HAL_SPI_Transmit(&hspi2, &pAddr, sizeof(pAddr), 100); */
  /* HAL_SPI_Receive(&hspi2, &pData, sizeof(pData), 100); */
  /* set(SS1); */
  /* print("USER_CTRL register: 0x%x\r\n", pData); */

  // Reset chip
  buf[0] = MPUREG_PWR_MGMT_1;
  buf[1] = BIT_H_RESET;
  reset(SS1);
  HAL_SPI_Transmit(&hspi2, buf, sizeof(buf), 100);
  /* HAL_SPI_Transmit(&hspi2, &pData, sizeof(pData), 100); */
  set(SS1);
  HAL_Delay(100);

  /* // Signal reset */
  buf[0] = MPUREG_SIGNAL_PATH_RESET;
  buf[1] = 0x07;
  reset(SS1);
  HAL_SPI_Transmit(&hspi2, buf, sizeof(buf), 100);
  set(SS1);
  HAL_Delay(100);

  /* // Wake up and set GYROZ clock */
  buf[0] = MPUREG_PWR_MGMT_1;
  buf[1] = MPU_CLK_SEL_PLLGYROZ;
  reset(SS1);
  HAL_SPI_Transmit(&hspi2, buf, sizeof(buf), 100);
  set(SS1);
  HAL_Delay(10);

  // read GYROZ clock
  /* pAddr = MPUREG_PWR_MGMT_1 | READ_FLAG; */
  /* reset(SS1); */
  /* HAL_SPI_Transmit(&hspi2, &pAddr, sizeof(pAddr), 100); */
  /* HAL_SPI_Receive(&hspi2, &pData, sizeof(pData), 100); */
  /* set(SS1); */
  /* HAL_Delay(10); */
  /* print("MPUREG_PWR_MGMT_1: 0x%x\r\n", pData); */

  /* // Disable I2C */
  // buf[0] = MPUREG_USER_CTRL;
  // buf[1] = BIT_I2C_IF_DIS;
  // reset(SS1);
  // HAL_SPI_Transmit(&hspi2, buf, sizeof(buf), 100);
  // set(SS1);
  // HAL_Delay(10);

  /* // Who am I? */
  pAddr = MPUREG_WHOAMI | READ_FLAG;
  pData = 0;
  reset(SS1);
  HAL_SPI_Transmit(&hspi2, &pAddr, sizeof(pAddr), 100);
  HAL_SPI_Receive(&hspi2, &pData, sizeof(pData), 100);
  set(SS1);
  print("WHOAMI: %hd\r\n", pData);
  if (pData != 104)
    Error_Handler();
  HAL_Delay(10);
  /* print("WHOAMI: %hd\r\n", who_am_i()); */

  /* // Set sample rate */
  buf[0] = MPUREG_SMPLRT_DIV;
  buf[1] = 1;
  reset(SS1);
  HAL_SPI_Transmit(&hspi2, buf, sizeof(buf), 100);
  set(SS1);
  HAL_Delay(10);

  /* // FS & DLPF */
  buf[0] = MPUREG_CONFIG;
  buf[1] = BITS_DLPF_CFG_188HZ;
  reset(SS1);
  HAL_SPI_Transmit(&hspi2, buf, sizeof(buf), 100);
  set(SS1);
  HAL_Delay(10);

  // buf[0] = MPUREG_INT_ENABLE;
  // buf[1] = 0x00;
  // reset(SS1);
  // HAL_SPI_Transmit(&hspi2, buf, sizeof(buf), 100);
  // set(SS1);
  // HAL_Delay(10);
}

void set_gyro_scale(void) {
  uint8_t buf[2];

  buf[0] = MPUREG_GYRO_CONFIG;
  buf[1] = BITS_FS_1000DPS;
  reset(SS1);
  HAL_SPI_Transmit(&hspi2, buf, sizeof(buf), 100);
  set(SS1);
  HAL_Delay(10);
}

float readGyro(void) {
  uint8_t pAddr = MPUREG_GYRO_ZOUT_H | READ_FLAG;
  int16_t bit_data;
  float data;
  uint8_t responseH = 0, responseL = 0;

  uint8_t interruptStatusAddr = MPUREG_INTERRUPT_STATUS | READ_FLAG;
  uint8_t interrupt_data = 0;

  reset(SS1);

  // while (interrupt_data & 0x1 != 1) {
  //   HAL_SPI_Transmit(&hspi2, &interruptStatusAddr,
  //   sizeof(interruptStatusAddr),
  //                    100);
  //   HAL_SPI_Receive(&hspi2, &interrupt_data, sizeof(interrupt_data), 100);
  // }

  HAL_SPI_Transmit(&hspi2, &pAddr, sizeof(pAddr), 100);
  int count_delay = 0;
  for (; count_delay < 1; count_delay++)
    ;
  // HAL_SPI_Receive(&hspi2, &bit_data, sizeof(bit_data), 100);
  HAL_SPI_Receive(&hspi2, &responseH, sizeof(responseH), 100);
  HAL_SPI_Receive(&hspi2, &responseL, sizeof(responseL), 100);
  bit_data = ((int16_t)responseH << 8) | responseL;

  data = (float)bit_data;
  data = data / 32.8;

  // From the data sheet:
  // https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf
  // page 12
  // data = (((int32_t)bit_data) * 10000 - (191942));
  set(SS1);

  /* return bit_data; */
  return data;
}

uint8_t who_am_i(void) {
  // Who am I?
  uint8_t pAddr = MPUREG_WHOAMI | READ_FLAG;
  uint8_t pData = 0;
  reset(SS1);

  uint8_t interruptStatusAddr = MPUREG_INTERRUPT_STATUS | READ_FLAG;
  uint8_t interrupt_data = 0;

  // while (interrupt_data & 0x1 != 1) {
  //   HAL_SPI_Transmit(&hspi2, &interruptStatusAddr,
  //   sizeof(interruptStatusAddr),
  //                    100);
  //   HAL_SPI_Receive(&hspi2, &interrupt_data, sizeof(interrupt_data), 100);
  // }

  HAL_SPI_Transmit(&hspi2, &pAddr, sizeof(pAddr), 100);
  int count_delay = 0;
  for (; count_delay < 1; count_delay++)
    ;
  HAL_SPI_Receive(&hspi2, &pData, sizeof(pData), 100);
  set(SS1);
  return pData;
}
