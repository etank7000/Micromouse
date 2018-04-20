#include "debug.h"
#include "controller.h"
#include "encoder.h"
#include "gyro.h"
#include "ir_sensor.h"
#include "usart.h"

#define DATA_SIZE 2001

static volatile int speedData[2][DATA_SIZE];
static volatile int dataNum = 0;
static int angle = 0;
static int baseGyroRef = 0;

void printSensorValues(void) {
  readReceivers();
  /* print("LF: %d  LD: %d  LH: %d  RH: %d  RD: %d  RF: %d\t" */
  /*       "lEnc: %lu  rEnc: %lu\r\n", */
  /*       getRecLF(), getRecLD(), getRecLH(), getRecRH(), getRecRD(),
   * getRecRF(), */
  /*       getLeftEnc(), getRightEnc() */
  /*       ); */
  /* print("lEnc: %lu  rEnc: %lu\r\n", getLeftEnc(), getRightEnc()); */
  // print("LF: %d  LD: %d  LH: %d  RH: %d  RD: %d  RF: %d\r\n",
  //       getRecLF(), getRecLD(), getRecLH(), getRecRH(), getRecRD(),
  //       getRecRF());
  print("LD: %d  RD: %d\r\n", getRecLD(), getRecRD());
}

void calibrateGyro(void) {
  baseGyroRef = 0;
  for (int i = 0; i < 10; i++) {
    baseGyroRef += readGyro();
  }
  baseGyroRef /= 10;
  print("%d\r\n", baseGyroRef);
}

void printGyroValues(void) {
  int outz = 0;
  for (int i = 0; i < 10; i++) {
    outz += (int)readGyro();
  }
  outz /= 10;
  int diff = outz - baseGyroRef;
  angle += diff / 21;
  print("%d\t", angle);
  print("%d\r\n", outz);
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
