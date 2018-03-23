#include "debug.h"
#include "ir_sensor.h"
#include "encoder.h"
#include "usart.h"
#include "controller.h"
#include "gyro.h"

#define DATA_SIZE 2001

static volatile int speedData[2][DATA_SIZE];
static volatile int dataNum = 0;

void printSensorValues(void)
{
  readReceivers();
  /* print("LF: %d  LD: %d  LH: %d  RH: %d  RD: %d  RF: %d\t" */
  /*       "lEnc: %lu  rEnc: %lu\r\n", */
  /*       getRecLF(), getRecLD(), getRecLH(), getRecRH(), getRecRD(), getRecRF(), */
  /*       getLeftEnc(), getRightEnc() */
  /*       ); */
  print("lEnc: %lu  rEnc: %lu\r\n", getLeftEnc(), getRightEnc());
}

void printGyroValues(void)
{
  int d = (int)readGyro();
  print("%d\r\n", d);
  HAL_Delay(100);
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
    print("%.4d,%.4d\r\n", speedData[0][i-1], speedData[1][i]);
  }
}