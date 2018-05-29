#include "debug.h"
#include "controller.h"
#include "encoder.h"
#include "gyro.h"
#include "ir_sensor.h"
#include "usart.h"
#include <stdlib.h>

#define DATA_SIZE 1 // 2001

static volatile int speedData[2][DATA_SIZE];
static volatile int dataNum = 0;
static float angle = 0;
static float baseGyroRef = 0;

void printSensorValues(void) {
  HAL_Delay(1);
  readReceivers();
  // print("lEnc: %lu  rEnc: %lu\r\n", getLeftEnc(), getRightEnc());
  print("LF: %d  LD: %d  LH: %d  RH: %d  RD: %d  RF: %d\r\n", getRecLF(),
        getRecLD(), getRecLH(), getRecRH(), getRecRD(), getRecRF());
}

void calibrateGyro(void) {
  baseGyroRef = 0;
  for (int i = 0; i < 15; i++) {
    baseGyroRef += readGyro();
  }
  baseGyroRef /= 10;
  print("GyroBaseRef: %d\r\n", (int)baseGyroRef);
}

void printGyroValues(void) {
  float outz = 0;
  // int time1 = HAL_GetTick();

  for (int i = 0; i < 15; i++) {
    outz += readGyro();
  }
  // int time2 = HAL_GetTick();
  // print("Time 1 is: %d and Time2 is: %d \r\n", time1, time2);
  // float elapsedTime = (time2 - time1) / ((float)1000);
  // print("elapsedTime is: %d \r\n", elapsedTime);
  outz /= 15;
  float diff = outz - baseGyroRef;
  // print("Difference is: %d\r\n", (int)diff);

  // Constant to multiply to make the gyro get 90 degree turns accurately!
  angle += diff * 1.12;
  // angle += diff / 10;
  print("%d   ", (int)angle);
  print("%d\r\n", (int)outz);
  HAL_Delay(100);
}

void updateSpeedData(void) {
  if (dataNum < DATA_SIZE) {
    speedData[0][dataNum] = (int)getCurSpeedX();
    speedData[1][dataNum] = (int)getEncSpeedX();
    dataNum++;
  }
}

void debugSpeedProfile(void) {
  moveUntilWall();
  int i;
  for (i = 1; i < DATA_SIZE; i++) {
    print("%.4d,%.4d\r\n", speedData[0][i - 1], speedData[1][i]);
  }
}
