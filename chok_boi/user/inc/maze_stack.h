/**
 * @file    maze_stack.h
 * @brief   Fixed size global cell stack.
 *
 * The stack can hold a maximum of 512 elements.
 */

/**
 * @brief A location in the maze, represented by (x, y).
 *
 * The mouse always starts in cell (0, 0).
 */
typedef struct cell
{
  unsigned char x;    /**< x-coordinate */
  unsigned char y;    /**< y-coordinate */
} cell;

/**
 * @brief Push a value onto the stack
 *
 * @param v The value to push
 * @retval 0 if successful, -1 if stack is full
 */
int stack_push(cell v);

/**
 * @brief Remove the element on top of the stack.
 *
 * @retval 0 if successful, -1 if stack is empty
 */
int stack_pop(void);

/**
 * @brief Access the top element in the stack.
 * @return The top element in the stack
 */
cell stack_top(void);

/**
 * @brief Check whether the stack is empty.
 *
 * @retval 1 if the stack is empty, 0 otherwise
 */
int stack_empty(void);
