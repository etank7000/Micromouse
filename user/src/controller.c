#include "controller.h"
#include "encoder.h"
#include "ir_sensor.h"
#include "motor.h"
#include "gpio.h"
#include <math.h>
#include "gyro.h"

// Conversion constants
static const int DIAMETER = 37; //35;    // Wheel diameter in millimeters
static const float PI = 3.14159265;
static const int STEPS_PER_REV = 3520; // Encoder steps per revolution
static const int CELL_WIDTH = 140;     // A cell has side length 180mm
static const int MOUSE_WIDTH = 69;     //74;  // The mouse has a width of 74mm
// Variable should be 5762 according to speed_to_counts calculation?

// TODO: Determine what this variable should be (not tested yet)
static const int CELL_ENC_COUNT = 5270; // Encoder counts per cell length
// OLD VALUE
// static const int CELL_ENC_COUNT = 5250; // Encoder counts per cell length

// Speed constants
// static const float MOVE_SPEED = 0.5; // m/s (or mm/ms)
static const float MOVE_SPEED = 0.5; // m/s (or mm/ms)
static const float MAX_SPEED = 1.0;  // m/s (or mm/ms), 1.0 is old

// Time constants
static const uint32_t PAUSE_TIME = 180; // ms
// static const uint32_t PAUSE_TIME = 228; // ms
static const uint32_t BIAS_TIME = 0;

// Acceleration constants
static const float ACC_X = 0.005; // Translational acceleration in mm/(ms)^2
static const float DEC_X = 0.005; // Translational deceleration in mm/(ms)^2
static const float ACC_W = 0.005; // Angular acceleration in mm/(ms)^2
static const float DEC_W = 0.005; // Angular deceleration in mm/(ms)^2

// Adjuster constants
static const int MOTOR_ADJUST_LIMIT = 210;
static const float ADJUST_DIVIDER = 1.01f;

// IR sensor constants
static const int SENSOR_DIVIDER = 112;

// OLD VALUES
// static const int SENSOR_DIVIDER = 90;   // increase for less correction, decrease for more correction
static const int LH_PUSH = 3320; // True mid: 2990
static const int RH_PUSH = 3320; // True mid: 2330
// OLD VALUES:
// static const int LH_PUSH = 3310; // True mid: 2990
// static const int RH_PUSH = 3315; // True mid: 2330

// // OLD VALUES
// static const int LH_PUSH = 3425; // True mid: 3303
// static const int RH_PUSH = 3240; // True mid: 3117

// LH/RH pull aren't really used anywhere
static const int LH_PULL = 2300;
static const int RH_PULL = 2300;

static const float PULL_FACTOR = 1.2f;

static const int LF_ADJUST = 3350; //3671;
static const int RF_ADJUST = 3350; //3644;

static const float ENCODER_CENTER = 0.47; // default is 0.55

// OLD VALUES
// static const int LF_ADJUST = 3433; //3671;
// static const int RF_ADJUST = 3576; //3644;
// LF: 900, RF: 1000 start to curve turn?

// Turn constants
static const float TURN_AROUND_MULTIPLIER = 1.029f;
// OLD VALUE
// static const float TURN_AROUND_MULTIPLIER = 1.025f;
static const float TURN_CONST_TIME = 0.996f;
// OLD VALUE
// static const float TURN_CONST_TIME = 0.996f;
static const float TURN_INPLACE_CONST_TIME = 0.9f;
// This value controls how much to curve turn
static const float TURN_CURVE_CONST_TIME = 0.85f;

// PID constants
static const float kpX = 2;
static const float kdX = 4;
static const float kpW = 1;
static const float kdW = 12;

// Speed variables
static float curSpeedX = 0; // Ideal current translational speed in counts/ms
static float curSpeedW = 0; // Ideal current angular speed in counts/ms
static int targetSpeedX = 0;
static int targetSpeedW = 0;

// Encoder variables
static int leftEncChange = 0;
static int rightEncChange = 0;
static volatile int encCount = 0;
static int encCountPrev = 0;
static int leftEncPrev = 0;
static int rightEncPrev = 0;

// PID variables
static float posErrorX = 0;
static float prevPosErrorX = 0;
static float posErrorW = 0;
static float prevPosErrorW = 0;

// TEST
static int firstCell = 1;
static int useSensorFeedback = 1;
static int finished = 0;
static int stop_flag = 0; // To account for initial motor bias to left
static int adjusting = 0;

// Private function prototypes
static float speed_to_counts(const float speed);
static float counts_to_speed(const float counts);
static void updateEncoderStatus(void);
static void updateCurrentSpeed(void);
static float decelerationRequired(int distanceLeft, float initialSpeed, float finalSpeed);
static int getSensorError(void);
static void calculateAdjust(void);
static void calculateMotorPwm(void);

void speedProfile(void)
{
  updateEncoderStatus();
  updateCurrentSpeed();
  if (adjusting)
  {
    calculateAdjust();
  }
  else
  {
    calculateMotorPwm();
  }
}

void resetSpeedProfile(void)
{
  reset(EM_LF);
  reset(EM_RF);
  reset(EM_H);
  reset(EM_D);
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
  encCount = 0;
  encCountPrev = 0;
  leftEncPrev = 0;
  rightEncPrev = 0;

  posErrorX = 0;
  prevPosErrorX = 0;
  posErrorW = 0;
  prevPosErrorW = 0;

  useSensorFeedback = 1;
  finished = 0;
  stop_flag = 0;
  adjusting = 0;

  reset(LED1);
  reset(LED2);
  reset(LED3);
}

void setFirstCell(void)
{
  firstCell = 1;
}

void moveUntilWall(void)
{
  encCount = 0;
  useSensorFeedback = 1;

  // Left motor bias correction
  // if (targetSpeedX == 0)
  //   stop_flag = 1;
  targetSpeedX = speed_to_counts(MOVE_SPEED);
  // if (stop_flag) {
  //   targetSpeedW = -targetSpeedX;
  //   HAL_Delay(BIAS_TIME);
  //   stop_flag = 0;
  // }
  targetSpeedW = 0;
  if (finished)
    return;
  HAL_Delay(1);
  while (!frontWallDetected())
  {
    if (encCount > CELL_ENC_COUNT)
    {
      encCount = 0;
      toggle(LED3);
    }
  }
}

// TODO: Finish/check implementation
void moveForward(float nCells)
{
  if (firstCell)
  {
    firstCell = 0;
    encCount = 0.25 * CELL_ENC_COUNT;
  }
  else
  {
    encCount = 0;
  }
  toggle(LED3);
  useSensorFeedback = 1;

  // Left motor bias correction
  // if (targetSpeedX == 0)
  //   stop_flag = 1;
  targetSpeedX = speed_to_counts(MOVE_SPEED);
  // if (stop_flag) {
  //   targetSpeedW = -1.8*targetSpeedX;
  //   HAL_Delay(BIAS_TIME);
  //   stop_flag = 0;
  // }

  targetSpeedW = 0;
  // HAL_Delay(1);
  int doneCount = nCells * CELL_ENC_COUNT;
  while (encCount < doneCount)
    ;
}

void turn(TurnDir turnDirection, TurnMotion turnMotion, int useGyro)
{

  float TURN_TIME = PI * CELL_WIDTH / (4 * MOVE_SPEED);
  if (turnMotion == CurveTurn)
    TURN_TIME *= TURN_CONST_TIME;
  float AT = ACC_W * TURN_TIME;
  float MAX_SPEED_W =
      (AT - sqrtf(AT * AT - 4 * AT * MOVE_SPEED * MOUSE_WIDTH / CELL_WIDTH)) / 2;
  float TURN_TIME_1 = MAX_SPEED_W / ACC_W;

  toggle(LED3);
  float maxSpeedW;
  if (turnDirection == LeftTurn)
    maxSpeedW = speed_to_counts(MAX_SPEED_W);
  else
    maxSpeedW = -speed_to_counts(MAX_SPEED_W);

  useSensorFeedback = 0;
  unsigned int startTime = HAL_GetTick();

  if (useGyro == 1)
  {
    if (turnMotion == InPlaceTurn)
    {
      resetGyroAngle();
      if (turnDirection == RightTurn)
      {
        while (fabsf(getGyroAngle()) <= 62.3)
        {
          targetSpeedW = maxSpeedW;
        }
        while (fabsf(getGyroAngle()) <= 92)
        {
          targetSpeedW = 0;
        }
      }

      if (turnDirection == LeftTurn)
      {
        while (fabsf(getGyroAngle()) <= 60.5)
        {
          targetSpeedW = maxSpeedW;
        }
        while (fabsf(getGyroAngle()) <= 84)
        {
          targetSpeedW = 0;
        }
      }
      resetGyroAngle();
    }
  }

  else
  {
    while (HAL_GetTick() - startTime < TURN_TIME - TURN_TIME_1)
    {
      targetSpeedW = maxSpeedW;
    }
    if (turnMotion == InPlaceTurn)
    {
      while (HAL_GetTick() - startTime < TURN_TIME * TURN_INPLACE_CONST_TIME)
      {
        targetSpeedW = 0;
      }
    }
    else
    {
      while (HAL_GetTick() - startTime < TURN_TIME * TURN_CURVE_CONST_TIME)
      {
        targetSpeedW = 0;
      }
    }
  }

  if (turnMotion == InPlaceTurn)
  {
    HAL_Delay(PAUSE_TIME);
  }
  toggle(LED3);
}

void turnAround(void)
{
  static const float TURN_TIME = PI * CELL_WIDTH / (2 * MOVE_SPEED) * TURN_AROUND_MULTIPLIER;
  static const float AT = ACC_W * TURN_TIME;
  static const float MAX_SPEED_W =
      (AT - sqrtf(AT * AT - 4 * AT * MOVE_SPEED * MOUSE_WIDTH / CELL_WIDTH)) / 2;
  static const float TURN_TIME_1 = MAX_SPEED_W / ACC_W;

  useSensorFeedback = 0;
  unsigned int startTime = HAL_GetTick();

  // resetGyroAngle();
  // while (fabsf(getGyroAngle()) <= 164)
  // {
  //   targetSpeedW = -speed_to_counts(MAX_SPEED_W);
  // }
  // while (fabsf(getGyroAngle()) <= 180)
  // {
  //   targetSpeedW = 0;
  // }
  // resetGyroAngle();

  while (HAL_GetTick() - startTime < TURN_TIME - TURN_TIME_1)
    targetSpeedW = -speed_to_counts(MAX_SPEED_W);

  while (HAL_GetTick() - startTime < TURN_TIME)
    targetSpeedW = 0;

  HAL_Delay(PAUSE_TIME);
  useSensorFeedback = 1;
}

// TODO: Implement this method
void stop(void)
{
  // Calculate when the mouse should start decelerating
  // If front wall detected, can use IR sensors to align with front wall
  targetSpeedX = 0;
  HAL_Delay(PAUSE_TIME);
}

void stopAtCellCenter(void)
{
  useSensorFeedback = 1;
  encCount = 0;
  targetSpeedW = 0;
  targetSpeedX = speed_to_counts(MOVE_SPEED);
  while (decelerationRequired(ENCODER_CENTER * CELL_ENC_COUNT - encCount, curSpeedX, 0) < speed_to_counts(DEC_X))
    ;
  targetSpeedX = 0;
  HAL_Delay(PAUSE_TIME);
}

void adjust(void)
{
  useSensorFeedback = 0;
  adjusting = 1;
  HAL_Delay(PAUSE_TIME);
  adjusting = 0;
}

float getEncSpeedX(void)
{
  return (rightEncChange + leftEncChange) / 2.0f;
}

float getCurSpeedX(void)
{
  return curSpeedX;
}

void testAdjust(void)
{
  calculateAdjust();
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

  encCount += (leftEncChange + rightEncChange) / 2;
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

// All units in encoder counts or encoder counts per ms
static float decelerationRequired(int distanceLeft, float initialSpeed, float finalSpeed)
{
  return fabsf((finalSpeed * finalSpeed - initialSpeed * initialSpeed) / (2 * distanceLeft));
}

static int getSensorError(void)
{
  if (!useSensorFeedback)
    return 0;
  int recLH = getRecLH();
  int recRH = getRecRH();
  if (leftWallDetected() && recLH > LH_PUSH)
    return recLH - LH_PUSH;
  if (rightWallDetected() && recRH > RH_PUSH)
    return RH_PUSH - recRH;
  // if (leftWallDetected() && recLH < LH_PULL)
  //   return (recLH - LH_PULL);
  // if (rightWallDetected() && recRH < RH_PULL)
  //   return (RH_PULL - recRH);
  return 0;
}

static void calculateAdjust(void)
{
  int leftAdjuster = (LF_ADJUST - getRecLF()) / ADJUST_DIVIDER;
  int rightAdjuster = (RF_ADJUST - getRecRF()) / ADJUST_DIVIDER;
  if (leftAdjuster > MOTOR_ADJUST_LIMIT)
    leftAdjuster = MOTOR_ADJUST_LIMIT;
  if (rightAdjuster > MOTOR_ADJUST_LIMIT)
    rightAdjuster = MOTOR_ADJUST_LIMIT;
  setLeftMotor(leftAdjuster);
  setRightMotor(rightAdjuster);
}

static void calculateMotorPwm(void)
{
  int encFeedbackX = rightEncChange + leftEncChange;
  int encFeedbackW = rightEncChange - leftEncChange;

  posErrorX += 2 * curSpeedX - encFeedbackX; // Integrate speed to get position
  // posErrorW += 2 * curSpeedW - encFeedbackW;
  posErrorW += 2 * curSpeedW - encFeedbackW - getSensorError() / SENSOR_DIVIDER;

  int posPwmX = kpX * posErrorX + kdX * (posErrorX - prevPosErrorX);
  int posPwmW = kpW * posErrorW + kdW * (posErrorW - prevPosErrorW);

  prevPosErrorX = posErrorX;
  prevPosErrorW = posErrorW;

  setLeftMotor(posPwmX - posPwmW);
  setRightMotor(posPwmX + posPwmW);
}
