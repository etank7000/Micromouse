#include "ir_sensor.h"
#include "delay.h"
#include "gpio.h"

static int32_t recLF = 0;
static int32_t recRF = 0;
static int32_t recLH = 0;
static int32_t recRH = 0;
static int32_t recLD = 0;
static int32_t recRD = 0;

// static const int32_t FRONT_TURN = 900;
static const int32_t HAS_FRONT = 666;
static const int32_t HAS_LEFT = 1900;   // Diagonal sensor threshold
static const int32_t HAS_RIGHT = 2600;  // Diagonal sensor

void readReceivers(void)
{
  recLF = readRecLF();
  recRF = readRecRF();
  recLH = readRecLH();
  recRH = readRecRH();
  recLD = readRecLD();
  recRD = readRecRD();

  uint32_t start = micros();

  set(EM_LF);
  while (micros() - start < 60);
  recLF = readRecLF() - recLF;
  reset(EM_LF);
  if (recLF < 0)
  {
    recLF = 0;
  }
  while (micros() - start < 140);

  set(EM_RF);
  while (micros() - start < 200);
  recRF = readRecRF() - recRF;
  reset(EM_RF);
  if (recRF < 0)
  {
    recRF = 0;
  }
  while (micros() - start < 280);

  set(EM_H);
  while (micros() - start < 340);
  recLH = readRecLH() - recLH;
  recRH = readRecRH() - recRH;
  reset(EM_H);
  if (recLH < 0)
  {
    recLH = 0;
  }
  if (recRH < 0)
  {
    recRH = 0;
  }
  while (micros() - start < 420);

  set(EM_D);
  while (micros() - start < 480);
  recLD = readRecLD() - recLD;
  recRD = readRecRD() - recRD;
  reset(EM_D);
  if (recLD < 0)
  {
    recLD = 0;
  }
  if (recRD < 0)
  {
    recRD = 0;
  }
}

int32_t getRecLF()
{
  return recLF;
}

int32_t getRecRF()
{
  return recRF;
}

int32_t getRecLH()
{
  return recLH;
}

int32_t getRecRH()
{
  return recRH;
}

int32_t getRecLD()
{
  return recLD;
}

int32_t getRecRD()
{
  return recRD;
}

int32_t frontWallDetected(void) 
{
  return recLF > HAS_FRONT && recRF > HAS_FRONT;
}

int32_t leftWallDetected(void)
{
  return recLH > HAS_LEFT;
}

int32_t rightWallDetected(void)
{
  return recRH > HAS_RIGHT;
}
