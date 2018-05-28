#include "maze.h"
#include "bit_vector_256.h"
#include "dir.h"
#include "pathfinder.h"
#include "ir_sensor.h"
#include "gpio.h"

#include "stm32f4xx_hal.h"
#include "main.h"

static BitVector256 wallsNS = {{0}};
static BitVector256 wallsEW = {{0}};
static Dir heading = NORTH;
static unsigned short mouseX = 0;
static unsigned short mouseY = 0;

/**
 * @brief Place a wall in direction d of the cell at location (x, y)
 * 
 * @retval None
 */ 
static void setClosed(unsigned short x, unsigned short y, Dir d) {
  switch (d) {
    case NORTH:
      return bitvector_clear(&wallsNS, x, y+1);
    case SOUTH:
      return bitvector_clear(&wallsNS, x, y);
    case EAST:
      return bitvector_clear(&wallsEW, x+1, y);
    case WEST:
      return bitvector_clear(&wallsEW, x, y);
    case INVALID:
    default:
      return;
  }
}

static void moveForward(void) {
  if (!isOpen(mouseX, mouseY, heading))
    return;

  switch (heading) {
    case NORTH:
      mouseY++;
      break;
    case SOUTH:
      mouseY--;
      break;
    case EAST:
      mouseX++;
      break;
    case WEST:
      mouseX--;
      break;
    case INVALID:
    default:
      break;
  }
}

static inline void turnClockwise(void) {
  heading = clockwise(heading);
}

static inline void turnCounterClockwise(void) {
  heading = counterClockwise(heading);
}

static inline void turnAround(void) {
  heading = opposite(heading);
}

static inline void updateWalls(void) {
  if (frontWallDetected()) {
    setClosed(mouseX, mouseY, heading);
  }
  if (leftWallDetected())
    setClosed(mouseX, mouseY, counterClockwise(heading));
  if (rightWallDetected())
    setClosed(mouseX, mouseY, clockwise(heading));
}

void initializeMaze(void) {
  // Assume no walls
  bitvector_setAll(&wallsNS);
  bitvector_setAll(&wallsEW);

  // Close the borders of the maze
  for (int i = 0; i < MAZE_LEN; i++) {
		setClosed(i, 0, SOUTH);
		setClosed(0, i, WEST);
		setClosed(i, MAZE_LEN - 1, NORTH);
		setClosed(MAZE_LEN - 1, i, EAST);
  }
  
  // print("\r\n");
  // for (int y = MAZE_LEN - 1; y >= 0; y--) {
  //   for (int x = 0; x < MAZE_LEN; x++) {
  //     if (isOpen(x, y, SOUTH))
  //       print("(");
  //     else 
  //       print("-");
  //   }
  //   print("\r\n");
  //   for (int x = 0; x < MAZE_LEN; x++) {
  //     if (isOpen(x, y, WEST))
  //       print(")");
  //     else 
  //       print("|");
  //   }
  //   print("\r\n");
  // }

  initializePathFinder();
}

void printMaze(void) {
  int n;
  print("\r\n");
  for (int y = MAZE_LEN - 1; y >= 0; y--) {
    for (int x = 0; x < MAZE_LEN; x++) {
      n = !isOpen(x, y, NORTH) + !isOpen(x, y, EAST) * 2
          + !isOpen(x, y, SOUTH) * 4 + !isOpen(x, y, WEST) * 8;
      // n = isOpen(x, y, NORTH);
      print("%2d ", n);
    }
    print("\r\n");
  }
}

void saveMazeInFlash(void) {
  set(LED1);
  HAL_Delay(1000);
  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR
    | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR);
  FLASH_Erase_Sector(FLASH_SECTOR_3, FLASH_VOLTAGE_RANGE_3);
  // if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 0x0800C000, weed) != HAL_OK) {
  //   HAL_FLASH_Lock();
  //   return;
  // }
  int vecLen = VECTOR_SIZE*VECTOR_SIZE / (8 * sizeof(uint16_t));
  for (int i = 0; i < vecLen; i++) {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,
              0x0800C000 + 2*i, wallsEW.vector[i]) != HAL_OK) {
                        Error_Handler();
                      }
  }
  for (int i = 0; i < vecLen; i++) {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 
              0x0800C000 + 2*(i + vecLen), wallsNS.vector[i]) != HAL_OK) {
                Error_Handler();
              }
  }

  HAL_FLASH_Lock();
  reset(LED1);
}

void readMazeFromFlash(void) {
  int vecLen = VECTOR_SIZE*VECTOR_SIZE / (8 * sizeof(uint16_t));
  for (int i = 0; i < vecLen; i++) {
    wallsEW.vector[i] = *(uint16_t *)(0x0800C000 + 2 * i);
  }
  for (int i = 0; i < vecLen; i++) {
    wallsNS.vector[i] = *(uint16_t *)(0x0800C000 + 2 * (i + vecLen));
  }
}

void resetMousePosition(void) {
  mouseX = 0;
  mouseY = 0;
  heading = NORTH;
}

int isOpen(unsigned short x, unsigned short y, Dir d) {
  switch (d) {
    case NORTH:
      return bitvector_get(&wallsNS, x, y+1);
    case SOUTH:
      return bitvector_get(&wallsNS, x, y);
    case EAST:
      return bitvector_get(&wallsEW, x+1, y);
    case WEST:
      return bitvector_get(&wallsEW, x, y);
    case INVALID:
    default:
      return 0;
  }
}

int wallInFront(void) {
  return !isOpen(mouseX, mouseY, heading);
}

int wallOnLeft(void) {
  return !isOpen(mouseX, mouseY, counterClockwise(heading));
}

int wallOnRight(void) {
  return !isOpen(mouseX, mouseY, clockwise(heading));
}

MouseMovement getNextMovement(void) {
  updateWalls();
  MouseMovement nextMove = nextMovement(mouseX, mouseY, heading);
  switch (nextMove) {
    case MoveForward:
      moveForward();
      break;
    case TurnClockwise:
      turnClockwise();
      moveForward();
      break;
    case TurnCounterClockwise:
      turnCounterClockwise();
      moveForward();
      break;
    case TurnAround:
      turnAround();
      moveForward();
      break;
    case Wait:
      break;
    case Finish:
    default:
      break;
  }
  return nextMove;
}
