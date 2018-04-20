#include "controller.h"
#include "encoder.h"
#include "ir_sensor.h"
#include "motor.h"
#include "gpio.h"
#include <math.h>

// Conversion constants
static const int DIAMETER = 35;    // Wheel diameter in millimeters
static const float PI = 3.14159;
static const int STEPS_PER_REV = 3520;   // Encoder steps per revolution
static const int CELL_WIDTH = 180;  // A cell has side length 180mm
static const int MOUSE_WIDTH = 74;  // The mouse has a width of 74mm

// Speed constants
static const float MOVE_SPEED = 0.5;   // m/s (or mm/ms)
static const float MAX_SPEED = 1.0;    // m/s (or mm/ms)

// Acceleration constants
static const float ACC_X = 0.005;   // Translational acceleration in mm/(ms)^2
static const float DEC_X = 0.005;   // Translational deceleration in mm/(ms)^2
static const float ACC_W = 0.005;   // Angular acceleration in mm/(ms)^2
static const float DEC_W = 0.005;   // Angular deceleration in mm/(ms)^2

// IR sensor threshold constants
static const int FRONT_WALL = 950;//1900;
static const int HAS_LEFT = 1100;
static const int HAS_RIGHT = 1100;
static const int LEFT_MID = 3600;
static const int RIGHT_MID = 3600;

static int useSensorFeedback = 1;

// PID constants
static const float kpX = 2;
static const float kdX = 4;
static const float kpW = 1;
static const float kdW = 12;

// Speed variables
static float curSpeedX = 0;   // Ideal current translational speed in counts/ms
static float curSpeedW = 0;   // Ideal current angular speed in counts/ms
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
static int getSensorError(void);
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

  curSpeedX = 0;
  curSpeedW = 0;
  targetSpeedX = 0;
  targetSpeedW = 0;

  leftEncChange = 0;
  rightEncChange = 0;
  leftEncCount = 0;
  rightEncCount = 0;
  encCount = 0;
  encCountPrev = 0;
  leftEncPrev = 0;
  rightEncPrev = 0;

  posErrorX = 0;
  prevPosErrorX = 0;
  posErrorW = 0;
  prevPosErrorW = 0;

  reset(LED1);
  reset(LED2);
  reset(LED3);
}

void moveUntilWall(void)
{
  targetSpeedW = 0;
  targetSpeedX = speed_to_counts(MOVE_SPEED);
  HAL_Delay(1);
  while (getRecLF() < FRONT_WALL || getRecRF() < FRONT_WALL);
  /* targetSpeedX = 0; */
  /* while (leftEncChange != 0 || rightEncChange != 0); */
  /* curSpeedX = 0; */
}

void turn(void)
{
  if (getRecLH() < HAS_LEFT)
    turnLeft();
  else if (getRecRH() < HAS_RIGHT)
    turnRight();
}

void turnRight(void)
{
  static const float TURN_TIME = PI * CELL_WIDTH / (4 * MOVE_SPEED);
  static const float AT = ACC_W * TURN_TIME;
  static const float MAX_SPEED_W = 
    (AT - sqrtf(AT*AT - 4*AT*MOVE_SPEED*MOUSE_WIDTH/CELL_WIDTH)) / 2;
  static const float TURN_TIME_1 = MAX_SPEED_W / ACC_W;

  useSensorFeedback = 0;
  unsigned int startTime = HAL_GetTick();

  while (HAL_GetTick() - startTime < TURN_TIME - TURN_TIME_1)
    targetSpeedW = -speed_to_counts(MAX_SPEED_W);

  while (HAL_GetTick() - startTime < TURN_TIME)
    targetSpeedW = 0;

  useSensorFeedback = 1;
}

void turnLeft(void)
{
  static const float TURN_TIME = PI * CELL_WIDTH / (4 * MOVE_SPEED);
  static const float AT = ACC_W * TURN_TIME;
  static const float MAX_SPEED_W = 
    (AT - sqrtf(AT*AT - 4*AT*MOVE_SPEED*MOUSE_WIDTH/CELL_WIDTH)) / 2;
  static const float TURN_TIME_1 = MAX_SPEED_W / ACC_W;

  useSensorFeedback = 0;
  unsigned int startTime = HAL_GetTick();

  while (HAL_GetTick() - startTime < TURN_TIME - TURN_TIME_1)
    targetSpeedW = speed_to_counts(MAX_SPEED_W);

  while (HAL_GetTick() - startTime < TURN_TIME)
    targetSpeedW = 0;

  useSensorFeedback = 1;
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
    curSpeedW += speed_to_counts(ACC_W);
    if (curSpeedW > targetSpeedW)
      curSpeedW = targetSpeedW;
  }
  else if (curSpeedW > targetSpeedW)
  {
    curSpeedW -= speed_to_counts(DEC_W);
    if (curSpeedW < targetSpeedW)
      curSpeedW = targetSpeedW;
  }
}

static int getSensorError(void)
{
  if (!useSensorFeedback)
    return 0;
  int recLH = getRecLH();
  int recRH = getRecRH();
  if (recLH > HAS_LEFT && recLH > LEFT_MID)
    return recLH - LEFT_MID;
  else if (recRH > HAS_RIGHT && recRH > RIGHT_MID)
    return RIGHT_MID - recRH;
  return 0;
}

static void calculateMotorPwm(void)
{
  int encFeedbackX = rightEncChange + leftEncChange;
  int encFeedbackW = rightEncChange - leftEncChange;

  posErrorX += 2*curSpeedX - encFeedbackX;  // Integrate speed to get position
  posErrorW += 2*curSpeedW - encFeedbackW - getSensorError() / 40;

  int posPwmX = kpX * posErrorX + kdX * (posErrorX - prevPosErrorX);
  int posPwmW = kpW * posErrorW + kdW * (posErrorW - prevPosErrorW);

  prevPosErrorX = posErrorX;
  prevPosErrorW = posErrorW;

  setLeftMotor(posPwmX - posPwmW);
  setRightMotor(posPwmX + posPwmW);
}
