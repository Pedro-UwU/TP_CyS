#ifndef ARGUMENT_PARSER_H
#define ARGUMENT_PARSER_H
#include <def.h>

// -embed
// -extract
// -in file
// -p bitmapfile
// -out bitmapfile
// -steg <LSB1 | LSB4 | LSBI>
// * -a <aes128, aes192, aes256, 3des>
// * -m <ecb | cfb | ofb | cbc>
// * -pass password
// 

typedef struct {
  bool embed;
  // FILE inFile
  // BMPFile carrier
  // FILE out
} Args ;

/*
 * args must be freed by the user
 */
Args* parse_arguments(int argc, char* argv[]);

#endif
