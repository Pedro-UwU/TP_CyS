#ifndef ARGUMENT_PARSER_H
#define ARGUMENT_PARSER_H
#include <def.h>
#include <bmp_files.h>
#include <input_file_processing.h>

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
        EncryptionAlgorithm algorithm;
        EncryptionMode mode;
        const char *password;
} Encryption;

typedef struct {
        Action action;
        BmpFile *carrier;
        InputData *in_file;
        char *out;
        LsbType lsb_type;
        Encryption encryption;
} Args;

/*
 * args must be freed by the user
 */
Args *parse_arguments(int argc, char *argv[]);
void free_args(Args *args);

#endif /* ARGUMENT_PARSER_H */
