#include "gyro.h"
#include "gpio.h"
#include "spi.h"
#include "usart.h"

#define GYRO_CALIB_COUNT 75

static float gyro_angle = 0;
static int32_t gyro_z_ref = 0;
static int32_t prev_gyro_z[2] = {0};
static int prev_time = 0;

void gyroSPIInit(void)
{
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

void setGyroScale(void)
{
  uint8_t buf[2];

  buf[0] = MPUREG_GYRO_CONFIG;
  // buf[1] = BITS_FS_1000DPS;
  buf[1] = BITS_FS_2000DPS;
  reset(SS1);
  HAL_SPI_Transmit(&hspi2, buf, sizeof(buf), 100);
  set(SS1);
  HAL_Delay(10);
}

int16_t readGyro(void)
{
  uint8_t pAddr = MPUREG_GYRO_ZOUT_H | READ_FLAG;
  int16_t bit_data = 0;
  uint8_t responseH = 0, responseL = 0;

  reset(SS1);

  HAL_SPI_Transmit(&hspi2, &pAddr, sizeof(pAddr), 100);
  int count_delay = 0;
  for (; count_delay < 1; count_delay++)
  {
  }
  // HAL_SPI_Receive(&hspi2, &bit_data, sizeof(bit_data), 100);
  HAL_SPI_Receive(&hspi2, &responseH, sizeof(responseH), 100);
  for (; count_delay < 4; count_delay++)
  {
  }

  pAddr = MPUREG_GYRO_ZOUT_L | READ_FLAG;
  HAL_SPI_Transmit(&hspi2, &pAddr, sizeof(pAddr), 100);
  for (; count_delay < 1; count_delay++)
  {
  }
  HAL_SPI_Receive(&hspi2, &responseL, sizeof(responseL), 100);
  for (; count_delay < 4; count_delay++)
  {
  }

  int16_t resH = (int16_t)responseH;
  bit_data = (resH << 8) | responseL;

  set(SS1);

  /* return bit_data; */
  return bit_data;
  // data = (float)bit_data;
  // data = (float)bit_data / 32.8;
  // data = data / 32.8;

  // print("Data: %d ", (int)data);
  // data = data / 16.4;

  // From the data sheet:
  // https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf
  // page 12
  // data = (((int32_t)bit_data) * 10000 - (191942));
}

void gyroWHOAMI(void)
{
  uint8_t pAddr;
  uint8_t pData;

  pAddr = MPUREG_WHOAMI | READ_FLAG;
  pData = 0;

  reset(SS1);
  HAL_SPI_Transmit(&hspi2, &pAddr, sizeof(pAddr), 100);
  int count_delay = 0;

  for (; count_delay < 1; count_delay++)
    ;
  HAL_SPI_Receive(&hspi2, &pData, sizeof(pData), 100);
  set(SS1);

  print("WHO AM I: %hd\r\n", pData);
}

void calibrateGyro(void)
{
  for (int i = 0; i < GYRO_CALIB_COUNT; i++)
  {
    gyro_z_ref += readGyro();
    // Gyro can only at max output at 1000 Hz, so wait 2 ms here for calibration
    HAL_Delay(2);
  }

  gyro_z_ref /= GYRO_CALIB_COUNT;
  prev_gyro_z[1] = gyro_z_ref;
  prev_gyro_z[0] = gyro_z_ref;
}

void updateGyroAngle(void)
{
  // since we can't do floats, the angle will be multiplied by 1000
  int16_t outz = 0;

  outz = readGyro();
  // float diff = (outz - gyro_z_ref) / 16.4 * 0.001;    // <--- so this doesnt really work

  // divide by 16.4, divide by 1000 since 1 ms = 1/1000 s, 2 since area of trapezoid
  // 1/(16.4*1000*2) = .000030488 -> 0.0000305
  int curr_time = HAL_GetTick();
  // float diff2 = (curr_time- prev_time) * (outz + prev_gyro_z - 2 * gyro_z_ref) * 0.000505;
  // float diff2 = 0.57 * (curr_time - prev_time) * (outz + prev_gyro_z[1] + prev_gyro_z[0] - 3 * gyro_z_ref) / 3 / 16.4 / 1000;
  float diff2 = (curr_time - prev_time) * (outz + prev_gyro_z[1] + prev_gyro_z[0] - 3 * gyro_z_ref) / 3 / 16.4 / 1000;
  // float diff2 = (curr_time - prev_time) * (outz - gyro_z_ref) * 0.000030488;

  gyro_angle += (diff2);
  prev_gyro_z[1] = prev_gyro_z[0];
  prev_gyro_z[0] = outz;
  prev_time = curr_time;
}

void resetGyroAngle(void)
{
  gyro_angle = 0;
}

float getGyroAngle(void)
{
  return gyro_angle;
}

int16_t getGyroOutZ(void)
{
  return prev_gyro_z[0];
}