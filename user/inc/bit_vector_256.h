#ifndef BIT_VECTOR_256
#define BIT_VECTOR_256

#include <stdint.h>
#include <string.h>

#define VECTOR_SIZE 16

typedef struct BitVector256 {
  uint16_t vector[(VECTOR_SIZE*VECTOR_SIZE) / (8 * sizeof(uint16_t))];
} BitVector256;

inline void bitvector_set(BitVector256* v, unsigned x, unsigned y) {
  if (x < VECTOR_SIZE && y < VECTOR_SIZE)
    v->vector[x] |= 1<<y;
}

inline void bitvector_clear(BitVector256* v, unsigned x, unsigned y) {
  if (x < VECTOR_SIZE && y < VECTOR_SIZE)
    v->vector[x] &= ~(1<<y);
}

inline int bitvector_get(BitVector256* v, unsigned x, unsigned y) {
  if (x < VECTOR_SIZE && y < VECTOR_SIZE)
    return (v->vector[x] & 1<<y) != 0;
  return 0;
}

inline void bitvector_clearAll(BitVector256* v) {
  memset(v->vector, 0, sizeof(v->vector));
}

inline void bitvector_setAll(BitVector256* v) {
  memset(v->vector, ~0, sizeof(v->vector));
}

#endif  // BIT_VECTOR_256
