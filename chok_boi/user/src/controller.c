#include "controller.h"
#include "encoder.h"
#include "ir_sensor.h"
#include "motor.h"
#include "gpio.h"

// Conversion constants
static const int DIAMETER = 35;    // Wheel diameter in millimeters
static const float PI = 3.14159;
static const int STEPS_PER_REV = 3520;   // Encoder steps per revolution

// Speed constants
static const float MOVE_SPEED = 0.5;   // m/s
static const float MAX_SPEED = 1.0;    // m/s

// Acceleration constants
static const float ACC_X = 0.005;   // Translational acceleration in mm/(ms)^2
static const float DEC_X = 0.005;   // Translational deceleration in mm/(ms)^2
static const float ACC_W = 0.5;   // Angular acceleration
static const float DEC_W = 0.5;   // Angular deceleration

// IR sensor threshold constants
static const int FRONT_WALL = 1200;

// PID constants
static const float kpX = 2;
static const float kdX = 4;
static const float kpW = 1;
static const float kdW = 12;

// Speed variables
static float curSpeedX = 0;   // Ideal current translational speed in counts/ms
static float curSpeedW = 0;   // Ideal current angular speed (units TBD)
static int targetSpeedX = 0;
static int targetSpeedW = 0;

// Encoder variables
static int leftEncChange = 0;
static int rightEncChange = 0;
static int leftEncCount = 0;
static int rightEncCount = 0;
static int encCount = 0;
static int encCountPrev = 0;
static int leftEncPrev = 0;
static int rightEncPrev = 0;

// PID variables
static float posErrorX = 0;
static float prevPosErrorX = 0;
static float posErrorW = 0;
static float prevPosErrorW = 0;

// Private function prototypes
static float speed_to_counts(const float speed);
static float counts_to_speed(const float counts);
static void updateEncoderStatus(void);
static void updateCurrentSpeed(void);
static void calculateMotorPwm(void);

void speedProfile(void)
{
  updateEncoderStatus();
  updateCurrentSpeed();
  calculateMotorPwm();
}

void resetSpeedProfile(void)
{
  reset(EM_LF);
  reset(EM_RF);
  reset(EM_H);
  reset(EM_D);
  reset(MODE);
  setLeftMotor(0);
  setRightMotor(0);
  resetLeftEnc();
  resetRightEnc();
  reset(LED1);
  reset(LED2);
  reset(LED3);
}

void moveUntilWall(void)
{
  targetSpeedW = 0;
  targetSpeedX = speed_to_counts(MOVE_SPEED);
  while (getRecLF() < FRONT_WALL || getRecRF() < FRONT_WALL);
  targetSpeedX = 0;
  while (leftEncChange != 0 || rightEncChange != 0);
}

float getEncSpeedX(void)
{
  return (rightEncChange + leftEncChange) / 2.0f;
}

float getCurSpeedX(void)
{
  return curSpeedX;
}

// Convert from mm/ms to encoder counts per millisecond.
static float speed_to_counts(const float speed)
{
  return speed / (DIAMETER * PI) * STEPS_PER_REV;
}

// Convert from encoder counts per millisecond to mm/ms.
static float counts_to_speed(const float counts)
{
  return counts * (DIAMETER * PI) / STEPS_PER_REV;
}

static void updateEncoderStatus(void)
{
  int leftEnc = getLeftEnc();
  int rightEnc = getRightEnc();

  leftEncChange = leftEnc - leftEncPrev;
  rightEncChange = rightEnc - rightEncPrev;

  leftEncPrev = leftEnc;
  rightEncPrev = rightEnc;

  leftEncCount += leftEncChange;
  rightEncCount += rightEncChange;
  encCount = (leftEncCount + rightEncCount) / 2;
}

static void updateCurrentSpeed(void)
{
  if (curSpeedX < targetSpeedX)
  {
    curSpeedX += speed_to_counts(ACC_X);
    if (curSpeedX > targetSpeedX)
      curSpeedX = targetSpeedX;
  }
  else if (curSpeedX > targetSpeedX)
  {
    curSpeedX -= speed_to_counts(DEC_X);
    if (curSpeedX < targetSpeedX)
      curSpeedX = targetSpeedX;
  }

  if (curSpeedW < targetSpeedW)
  {
    curSpeedW += ACC_W;
    if (curSpeedW > targetSpeedW)
      curSpeedW = targetSpeedW;
  }
  else if (curSpeedW > targetSpeedW)
  {
    curSpeedW -= DEC_W;
    if (curSpeedW < targetSpeedW)
      curSpeedW = targetSpeedW;
  }
}

static void calculateMotorPwm(void)
{
  int encFeedbackX = rightEncChange + leftEncChange;
  int encFeedbackW = rightEncChange - leftEncChange;

  posErrorX += 2*curSpeedX - encFeedbackX;  // Integrate speed to get position
  posErrorW += curSpeedW - encFeedbackW;

  int posPwmX = kpX * posErrorX + kdX * (posErrorX - prevPosErrorX);
  int posPwmW = kpW * posErrorW + kdW * (posErrorW - prevPosErrorW);

  prevPosErrorX = posErrorX;
  prevPosErrorW = posErrorW;

  setLeftMotor(posPwmX - posPwmW);
  setRightMotor(posPwmX + posPwmW);
}
