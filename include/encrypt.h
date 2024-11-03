#ifndef ENCRYPT_H
#define ENCRYPT_H
#include <def.h>
#include <argument_parser.h>

#define ENCRYPT_DECRYPT_BUFFER_SIZE 256 * 1024 * 1024 // 256 MiB

unsigned char *encrypt_payload(Encryption *encryption, const unsigned char *payload,
                               size_t payload_size, size_t *out_size);

unsigned char *decrypt_payload(Encryption *encryption, const unsigned char *payload,
                               size_t payload_size, size_t *out_size);

#endif /* ENCRYPT_H */
