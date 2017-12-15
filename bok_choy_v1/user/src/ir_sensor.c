#include "ir_sensor.h"
#include "delay.h"
#include "gpio.h"

static int32_t recLF = 0;
static int32_t recRF = 0;
static int32_t recLS = 0;
static int32_t recRS = 0;

void readReceivers(void)
{
  recLF = readRecLF();
  recRF = readRecRF();
  recLS = readRecLS();
  recRS = readRecRS();

  uint32_t start = micros();

  turnOn(EM_LF);
  while (micros() - start < 60);
  recLF = readRecLF() - recLF;
  turnOff(EM_LF);
  if (recLF < 0)
  {
    recLF = 0;
  }
  while (micros() - start < 140);

  turnOn(EM_RF);
  while (micros() - start < 200);
  recRF = readRecRF() - recRF;
  turnOff(EM_RF);
  if (recRF < 0)
  {
    recRF = 0;
  }
  while (micros() - start < 280);

  turnOn(EM_S);
  while (micros() - start < 340);
  recLS = readRecLS() - recLS;
  recRS = readRecRS() - recRS;
  turnOff(EM_S);
  if (recLS < 0)
  {
    recLS = 0;
  }
  if (recRS < 0)
  {
    recRS = 0;
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

int32_t getRecLS()
{
  return recLS;
}

int32_t getRecRS()
{
  return recRS;
}
