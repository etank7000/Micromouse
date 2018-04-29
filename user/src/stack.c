#include "stack.h"

#define MAX_SIZE 512

static Cell stack[MAX_SIZE];
static int size = 0;

int stack_push(unsigned short x, unsigned short y)
{
  if (size >= MAX_SIZE)
  {
    return -1;
  }
  Cell c = {x, y};
  stack[size++] = c;
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

Cell stack_top(void)
{
  return stack[size - 1];
}

int stack_empty(void)
{
  return size == 0;
}
