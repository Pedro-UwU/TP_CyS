#ifndef EXTRACT
#define EXTRACT
#include "encrypt.h"
#include <argument_parser.h>
#include <bmp_files.h>
#include <def.h>
#include <extract.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lsbi.h>

uint32_t get_payload_message_lenght(uint8_t *payload, uint8_t step);
uint32_t get_message_lenght(uint8_t *message);

char extract_char(uint8_t *payload, uint32_t index, uint8_t step);
char *extract_message(uint8_t *payload, uint32_t message_size, uint8_t step);
static void extract_encrypted_msg(Args *args, uint8_t step);
char *extract_payload_extension(uint8_t *payload, uint32_t message_size, uint8_t step);
char *extract_message_extension(unsigned char *message, uint32_t message_size);
void save_extracted_file(Args *args, char *payload, uint32_t payload_length, char *extension);

static uint8_t *lsbi_extract_pattern_map(uint8_t *payload, size_t *payload_size,
                                         uint8_t pattern_map[N_LSBI_Pattern]);
uint8_t *reverse_lsbi_bits(uint8_t *payload, size_t payload_size, uint8_t *pattern_map);
uint8_t *extract_message_lsbi(uint8_t *payload, size_t payload_size);

void handle_extraction_lsb(Args *args, uint8_t step);
void handle_lsbi_extraction(Args *args);

void handle_extraction(Args *args)
{
        if (args == NULL) {
                printf("[ERROR] - extract_message_from_file - NULL argument struct\n");
                exit(1);
        }

        if (args->encryption.algorithm == EncryptAlgo_NONE) {
                switch (args->lsb_type) {
                case LSB1:
                        handle_extraction_lsb(args, 1);
                        break;
                case LSB4:
                        handle_extraction_lsb(args, 4);
                        break;
                case LSBI:
                        handle_lsbi_extraction(args);
                        break;
                }
        } else {
                switch (args->lsb_type) {
                case LSB1:
                        extract_encrypted_msg(args, 1);
                        break;
                case LSB4:
                        extract_encrypted_msg(args, 4);
                        break;
                case LSBI:
                        handle_lsbi_extraction(args);
                        break;
                }
        }
}

static void extract_encrypted_msg(Args *args, uint8_t step)
{
        BmpFile *carrier = args->carrier;
        uint8_t *payload = carrier->payload;

        uint32_t e_message_size = get_payload_message_lenght(payload, step);
        char *e_message = extract_message(payload, e_message_size, step);

        size_t d_size = 0;
        unsigned char *decrypted = decrypt_payload(&args->encryption, (unsigned char *)e_message,
                                                   e_message_size, &d_size);

        uint32_t message_size = get_message_lenght(decrypted);
        unsigned char *message = decrypted + DWORD;
        char *extension = extract_message_extension(message, message_size);

        save_extracted_file(args, (char *)message, message_size, extension);

        free(decrypted);
        free(e_message);
}

void handle_extraction_lsb(Args *args, uint8_t step)
{
        BmpFile *carrier = args->carrier;
        uint8_t *payload = carrier->payload;
        uint32_t message_size = get_payload_message_lenght(payload, step);
        char *message = extract_message(payload, message_size, step);
        char *extension = extract_payload_extension(payload, message_size, step);
        save_extracted_file(args, message, message_size, extension);
        free(message);
        free(extension);
}

char extract_char(uint8_t *payload, uint32_t index, uint8_t step)
{
        uint32_t total_steps = sizeof(char) * 8 / step;
        uint8_t c = 0;
        for (uint32_t i = 0; i < total_steps; i++) {
                uint8_t bits = payload[index + i];
                bits &= ((1 << step) - 1);
                c <<= step;
                c |= bits;
        }
        return (char)c;
}

char *extract_message(uint8_t *payload, uint32_t message_size, uint8_t step)
{
        char *message = malloc(message_size * sizeof(char) + 1);
        if (message == NULL) {
                printf("[ERROR] - extract_message - Couldn't allocate memory for message\n");
                exit(1);
        }
        message[message_size] = '\0';
        for (uint32_t i = 0; i < message_size; i++) {
                char c = extract_char(payload, BITS_FOR_SIZE / step + i * sizeof(char) * 8 / step,
                                      step);
                message[i] = c;
        }
        return message;
}

static uint8_t *lsbi_extract_pattern_map(uint8_t *payload, size_t *payload_size,
                                         uint8_t pattern_map[N_LSBI_Pattern])
{
        for (int i = 0; i < BYTE * N_LSBI_Pattern; i++) {
                uint8_t byte = payload[i];
                pattern_map[i] = byte & 0x01;
        }

        *payload_size -= BYTE * N_LSBI_Pattern;

        return payload + BYTE * N_LSBI_Pattern;
}

uint8_t *reverse_lsbi_bits(uint8_t *payload, size_t payload_size, uint8_t *pattern_map)
{
        for (uint32_t i = 0; i < payload_size; i += 1) {
                if ((i + 2) % 3 == 0) {
                        // Skip RED byte. Remember: its in BGR
                        continue;
                }

                uint32_t b = (payload[i] & LSBI_BYTE_PATTERN_MASK) >> 1;
                if (pattern_map[b] == 0) {
                        // Not inverted
                        continue;
                }

                /* PAPER
                 *
                 * Finally, we inverse the last bit of the stego-
                 * image, if the number of pixels that have 
                 * changed in specific patterns are greater than
                 * the number of pixels that are not changed.
                 */
                if (payload[i] & 0x01) {
                        payload[i] &= 0xFF ^ 0x01;
                } else {
                        payload[i] |= 0x01;
                }
        }

        return payload;
}

uint8_t *extract_message_lsbi(uint8_t *payload, size_t payload_size)
{
        // Message will be at most as big as the payload
        uint8_t *message_from_payload = calloc(payload_size, sizeof(uint8_t));
        if (message_from_payload == NULL) {
                printf("[ERROR] - extract_message_lsbi - Failed to allocate memory for LSBI message\n");
                return NULL;
        }

        uint8_t byte[8] = { 0 };
        for (uint32_t i = 0, j = 0, k = 0; i < payload_size; i++) {
                if ((i + 2) % 3 == 0) {
                        // Skip RED byte. Remember: its in BGR
                        continue;
                }

                byte[j] = payload[i] & 0x01;
                j++;

                if (j < 8) {
                        continue;
                }

                // Bits to byte
                message_from_payload[k] = byte[0] << 7;
                message_from_payload[k] |= byte[1] << 6;
                message_from_payload[k] |= byte[2] << 5;
                message_from_payload[k] |= byte[3] << 4;
                message_from_payload[k] |= byte[4] << 3;
                message_from_payload[k] |= byte[5] << 2;
                message_from_payload[k] |= byte[6] << 1;
                message_from_payload[k] |= byte[7] << 0;

                j = 0;
                k++;
        }

        return message_from_payload;
}

void handle_lsbi_extraction(Args *args)
{
        BmpFile *carrier = args->carrier;
        size_t payload_size = carrier->info_header->sizeImage;

        uint8_t *payload = carrier->payload;

        // Extraer pattern map con LSB1
        uint8_t pattern_map[N_LSBI_Pattern] = { 0 };
        payload = lsbi_extract_pattern_map(payload, &payload_size, pattern_map);

        // Reverse LSBI bits in payload
        payload = reverse_lsbi_bits(payload, payload_size, pattern_map);

        uint8_t *full_message = extract_message_lsbi(payload, payload_size);
        if (full_message == NULL) {
                printf("[ERROR] - handle_lsbi_extraction - Message could not be extracted using LSBI");
                goto end;
        }

        uint32_t message_size;
        unsigned char *message;
        char *extension;
        unsigned char *decrypted = NULL;

        if (args->encryption.algorithm == EncryptAlgo_NONE) {
                message_size = get_message_lenght(full_message);
                message = full_message + DWORD; // Skip size
                extension = extract_message_extension(message, message_size);
        } else {
                uint32_t e_message_size = get_message_lenght(full_message);
                uint8_t *e_message = full_message + DWORD; // Skip size

                size_t d_size = 0;
                decrypted = decrypt_payload(&args->encryption, (unsigned char *)e_message,
                                            e_message_size, &d_size);

                message_size = get_message_lenght(decrypted);
                message = decrypted + DWORD;
                extension = extract_message_extension(message, message_size);
        }

        save_extracted_file(args, (char *)message, message_size, extension);

end:
        free(full_message);
        free(decrypted);
}

uint32_t get_payload_message_lenght(uint8_t *payload, uint8_t step)
{
        uint8_t total_steps = BITS_FOR_SIZE / step;
        uint32_t size = 0;
        for (uint8_t i = 0; i < total_steps; i++) {
                uint8_t bits = payload[i];
                bits &= ((1 << step) - 1);
                size <<= step;
                size |= bits;
        }
        return size;
}

uint32_t get_message_lenght(uint8_t *message)
{
        uint32_t size = 0;
        size |= ((uint32_t)message[0] << 24);
        size |= ((uint32_t)message[1] << 16);
        size |= ((uint32_t)message[2] << 8);
        size |= ((uint32_t)message[3]);
        return size;
}

char *extract_payload_extension(uint8_t *payload, uint32_t message_size, uint8_t step)
{
        char *extension = malloc(MAX_EXTENSION_LENGTH * sizeof(char));
        memset(extension, 0xFF, MAX_EXTENSION_LENGTH);
        if (extension == NULL) {
                printf("[ERROR] - extract_payload_extension - Couldn't allocate memory for extension\n");
                exit(1);
        }
        uint32_t i = 0;
        unsigned char last_read = 0xFF;
        for (i = 0; last_read != '\0'; i++) {
                char c = extract_char(
                        payload,
                        BITS_FOR_SIZE / step + (i + message_size) * sizeof(char) * 8 / step, step);
                extension[i] = c;
                last_read = c;
        }
        char *to_return = malloc(i * sizeof(char));
        memcpy(to_return, extension, i);
        free(extension);
        return to_return;
}

char *extract_message_extension(unsigned char *message, uint32_t message_size)
{
        return (char *)(message + message_size);
}

void save_extracted_file(Args *args, char *payload, uint32_t payload_length, char *extension)
{
        if (args == NULL || payload == NULL || extension == NULL) {
                printf("[ERROR] - save_extracted_file - Trying to save file with NULL arguments\n");
                exit(1);
        }
        char *path = args->out;
        char *full_path = malloc((strlen(path) + strlen(extension) + 1) * sizeof(char));
        strcpy(full_path, path);
        strcat(full_path, extension);
        FILE *output_file = fopen(full_path, "w");
        if (output_file == NULL) {
                printf("[ERROR] - save_extracted_file - Couldn't create file %s\n", full_path);
                exit(1);
        }
        if (fwrite(payload, sizeof(char), payload_length, output_file) != payload_length) {
                printf("[ERROR] - save_extracted_file - Couldn't write to output file\n");
                exit(1);
        }
        printf("Created file: %s\n", full_path);
        fclose(output_file);
        free(full_path);
}

#endif
