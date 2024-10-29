#ifndef ARGUMENT_PARSER_H
#define ARGUMENT_PARSER_H
#include <def.h>
#include <bmp_files.h>
#include <lsb.h>

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
  BmpFile* carrier;
  LsbData* in_file;
  // char* out
} Args ;

/*
 * args must be freed by the user
 */
Args* parse_arguments(int argc, char* argv[]);

#endif
