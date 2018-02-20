#include "maze_stack.h"

#define MAX_SIZE 512

static cell stack[MAX_SIZE];
static int size = 0;

int stack_push(cell v)
{
  if (size >= MAX_SIZE)
  {
    return -1;
  }
  stack[size++] = v;
  return 0;
}

int stack_pop(void)
{
  if (size <= 0)
  {
    return -1;
  }
  size--;
  return 0;
}

cell stack_top(void)
{
  return stack[size - 1];
}

int stack_empty(void)
{
  return size == 0;
}
