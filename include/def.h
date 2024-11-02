#ifndef DEF_H
#define DEF_H
#define TRUE 1
#define FALSE 0
#define BITS_FOR_SIZE 32
#define BYTES_FOR_SIZE (BITS_FOR_SIZE / 8)
#define MAX_EXTENSION_LENGTH 64

#include <stdint.h>

typedef uint8_t bool;

typedef enum { LSB1, LSB4, LSBI } LsbType;
typedef enum { ACTION_EMBED, ACTION_EXTRACT } Action;

typedef enum {
        EncryptAlgo_NONE,
        EncryptAlgo_AES128,
        EncryptAlgo_AES192,
        EncryptAlgo_AES256,
        EncryptAlgo_3DES
} EncryptionAlgorithm;

typedef enum {
        EncryptMode_NONE,
        EncryptMode_ECB,
        EncryptMode_CFB,
        EncryptMode_OFB,
        EncryptMode_CBC
} EncryptionMode;

#endif /* DEF_H */
