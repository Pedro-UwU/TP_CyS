#ifndef DEF_H
#define DEF_H
#define TRUE 1
#define FALSE 0
#define BITS_FOR_SIZE 32
#define BYTES_FOR_SIZE (BITS_FOR_SIZE/8)

#include <stdint.h>


typedef uint8_t bool;

typedef enum {
  LSB1,
  LSB4,
  LSBI
} LsbType;



#endif
