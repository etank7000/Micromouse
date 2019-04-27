#include "debug.h"
#include "controller.h"
#include "encoder.h"
#include "gyro.h"
#include "ir_sensor.h"
#include "usart.h"
#include <stdlib.h>

#define DATA_SIZE 1 // 2001
#define CALIBRATION_COUNT 25

static volatile int speedData[2][DATA_SIZE];
static volatile int dataNum = 0;
static int16_t angle = 0;
static int16_t baseGyroRef = 0;

void printSensorValues(void)
{
  HAL_Delay(1);
  readReceivers();
  // print("lEnc: %lu  rEnc: %lu\r\n", getLeftEnc(), getRightEnc());
  print("LF: %d  LD: %d  LH: %d  RH: %d  RD: %d  RF: %d\r\n", getRecLF(),
        getRecLD(), getRecLH(), getRecRH(), getRecRD(), getRecRF());
}

void printAngleValues(void)
{
  HAL_Delay(1);
  updateGyroAngle();
  int gyro_angle = (int)(getGyroAngle() * 1000);
  print("Angle: %d  \r\n", gyro_angle);
}

void calibrateGyroTest(void)
{
  baseGyroRef = 0;
  for (int i = 0; i < CALIBRATION_COUNT; i++)
  {
    baseGyroRef += readGyro();
    // HAL_Delay(1);
  }
  baseGyroRef /= CALIBRATION_COUNT;
  print("GyroBaseRef: %d\r\n", (int)baseGyroRef);
}

void printGyroValues(void)
{
  int16_t outz = 0;
  int time1 = HAL_GetTick();

  // for (int i = 0; i < CALIBRATION_COUNT; i++)
  // {
  //   outz += readGyro();
  // }
  int time2 = HAL_GetTick();
  // print("Time 1 is: %d and Time2 is: %d \r\n", time1, time2);

  // this is too small (like in the 1 ms range )
  float elapsedTime = (time2 - time1) / ((float)1000);
  // print("elapsedTime is: %d \r\n", elapsedTime);
  // outz /= CALIBRATION_COUNT;

  outz = readGyro();
  int16_t diff = (outz - baseGyroRef) / 16.4;
  // print("Difference is: %d\r\n", (int)diff);

  // Constant to multiply to make the gyro get 90 degree turns accurately!
  angle += diff;
  // angle += diff * 1.12;
  // angle += diff / 10;
  print("Angle: %d   ", (int)angle);
  print("Raw: %d\r\n", (int)diff);
  // HAL_Delay(100);
}

void updateSpeedData(void)
{
  if (dataNum < DATA_SIZE)
  {
    speedData[0][dataNum] = (int)getCurSpeedX();
    speedData[1][dataNum] = (int)getEncSpeedX();
    dataNum++;
  }
}

void debugSpeedProfile(void)
{
  moveUntilWall();
  int i;
  for (i = 1; i < DATA_SIZE; i++)
  {
    print("%.4d,%.4d\r\n", speedData[0][i - 1], speedData[1][i]);
  }
}
