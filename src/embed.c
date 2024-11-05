// embed.c (versión completa)
#ifndef EMBED
#define EMBED
#include <argument_parser.h>
#include <bmp_files.h>
#include <def.h>
#include <embed.h>
#include <input_file_processing.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <encrypt.h>
#include <lsbi.h>

unsigned char get_isolated_bits(unsigned char value, size_t index, size_t length);
void inject_message(unsigned char *dest, size_t dest_size, const unsigned char *msg, size_t msg_dim,
                    size_t step);
void inject_lsbi_message(unsigned char *dest, size_t dest_size, const unsigned char *msg,
                         size_t msg_dim, size_t header_size);

void handle_lsb1(Args *args);
void handle_lsb4(Args *args);
void handle_lsbi(Args *args);

static void lsbi_apply_pattern_map_to_payload(uint8_t *payload, size_t size,
                                              size_t pattern_count[N_LSBI_Pattern],
                                              size_t pattern_changes[N_LSBI_Pattern]);
static void lsbi_flip_last_bit(uint8_t *payload, size_t size, size_t pattern_index);
static void lsbi_calculate_patterns(uint8_t *org, uint8_t *payload, size_t size,
                                    size_t count[N_LSBI_Pattern], size_t changes[N_LSBI_Pattern]);

// Funciones auxiliares para LSBI
static void inject_pattern_map(unsigned char *dest, uint8_t pattern_map)
{
        // Guarda el mapa de patrones usando LSB1 en los primeros 4 bytes
        for (int i = 0; i < 4; i++) {
                unsigned char bit = (pattern_map >> (3 - i)) & 0x01;
                dest[i] &= 0xFE; // Clear LSB
                dest[i] |= bit; // Set LSB to pattern bit
        }
}

static uint8_t check_pixel_changes(unsigned char *original, unsigned char *modified, size_t length,
                                   uint8_t pattern)
{
        int changed = 0, unchanged = 0;

        for (size_t i = 0; i < length; i++) {
                uint8_t pixel_pattern = (original[i] >> 1) & 0x03;
                if (pixel_pattern == pattern) {
                        if (original[i] != modified[i]) {
                                changed++;
                        } else {
                                unchanged++;
                        }
                }
        }

        return changed > unchanged;
}

static void invert_pattern_bits(unsigned char *data, size_t length, uint8_t pattern)
{
        for (size_t i = 0; i < length; i++) {
                uint8_t pixel_pattern = (data[i] >> 1) & 0x03;
                if (pixel_pattern == pattern) {
                        data[i] ^= 0x01; // Invierte el LSB
                }
        }
}

void handle_embedding(Args *args)
{
        if (args == NULL) {
                printf("[ERROR] - handle_embedding - NULL argument struct\n");
                exit(1);
        }
        switch (args->lsb_type) {
        case LSB1:
                handle_lsb1(args);
                break;
        case LSB4:
                handle_lsb4(args);
                break;
        case LSBI:
                handle_lsbi(args);
                break;
        default:
                break;
        }
}

void handle_lsb1(Args *args)
{
        InputData *input_data = args->in_file;
        BmpFile *bmp = args->carrier;
        unsigned char *payload;
        size_t dim = 0;

        if (args->encryption.algorithm == EncryptAlgo_NONE) {
                payload = generate_unencrypted_payload(input_data, &dim);
        } else {
                size_t p_dim = 0;
                unsigned char *p_payload;

                p_payload = generate_unencrypted_payload(input_data, &p_dim);
                payload = encrypt_payload(&args->encryption, p_payload, p_dim, &dim);
                free(p_payload);
        }

        inject_message(bmp->payload, bmp->info_header->sizeImage, payload, dim, 1);
        save_bmp(bmp, args->out);

        free(payload);
}

void handle_lsb4(Args *args)
{
        InputData *input_data = args->in_file;
        BmpFile *bmp = args->carrier;
        unsigned char *payload;
        size_t dim = 0;

        if (args->encryption.algorithm == EncryptAlgo_NONE) {
                payload = generate_unencrypted_payload(input_data, &dim);
        } else {
                size_t p_dim = 0;
                unsigned char *p_payload;

                p_payload = generate_unencrypted_payload(input_data, &p_dim);
                payload = encrypt_payload(&args->encryption, p_payload, p_dim, &dim);
                free(p_payload);
        }

        inject_message(bmp->payload, bmp->info_header->sizeImage, payload, dim, 4);
        save_bmp(bmp, args->out);

        free(payload);
}

void handle_lsbi(Args *args)
{
        InputData *input_data = args->in_file;
        BmpFile *bmp = args->carrier;
        unsigned char *payload;
        size_t dim = 0;
        unsigned char *bmp_payload_copy = clone_bmp_payload(bmp);

        if (args->encryption.algorithm == EncryptAlgo_NONE) {
                payload = generate_unencrypted_payload(input_data, &dim);
        } else {
                size_t p_dim = 0;
                unsigned char *p_payload;

                p_payload = generate_unencrypted_payload(input_data, &p_dim);
                payload = encrypt_payload(&args->encryption, p_payload, p_dim, &dim);
                free(p_payload);
        }

        // Aplicar LSB1 estándar primero
        inject_lsbi_message(bmp->payload, bmp->info_header->sizeImage, payload, dim, 4);

        // Analizar cambios por patrón
        size_t pattern_count[N_LSBI_Pattern] = { 0 };
        size_t pattern_changes[N_LSBI_Pattern] = { 0 };

        lsbi_calculate_patterns(bmp_payload_copy, bmp->payload, bmp->info_header->sizeImage,
                                pattern_count, pattern_changes);

        lsbi_apply_pattern_map_to_payload(bmp->payload, bmp->info_header->sizeImage, pattern_count,
                                          pattern_changes);

        // Guardar resultado
        save_bmp(bmp, args->out);

lsbi_end:
        free(payload);
        free(bmp_payload_copy);
}

static void lsbi_apply_pattern_map_to_payload(uint8_t *payload, size_t size,
                                              size_t pattern_count[N_LSBI_Pattern],
                                              size_t pattern_changes[N_LSBI_Pattern])
{
        /* PAPER
         *
         * Finally, we inverse the last bit of the stego-
         * image, if the number of pixels that have 
         * changed in specific patterns are greater than
         * the number of pixels that are not changed.
         */

        for (size_t i = 0; i < BYTE * N_LSBI_Pattern; i++) {
                if (pattern_changes[i] > pattern_count[i] - pattern_changes[i]) {
                        lsbi_flip_last_bit(payload, size, i);
                        payload[i] |= 0x01;
                } else {
                        payload[i] &= 0xFF ^ 0x01;
                }
        }
}

static void lsbi_flip_last_bit(uint8_t *payload, size_t size, size_t pattern_index)
{
        for (size_t j = N_LSBI_Pattern; j < size; j++) {
                uint32_t p = (payload[j] & LSBI_BYTE_PATTERN_MASK) >> 1;

                if ((j + 1) % 3 == 0 || p != pattern_index) {
                        continue;
                }
                // Invert last bit
                payload[j] ^= 0x01;
        }
}

static void lsbi_calculate_patterns(uint8_t *org, uint8_t *payload, size_t size,
                                    size_t count[N_LSBI_Pattern], size_t changes[N_LSBI_Pattern])
{
        for (size_t i = N_LSBI_Pattern; i < size; i++) {
                uint32_t p = (org[i] & LSBI_BYTE_PATTERN_MASK) >> 1;
                count[p]++;

                if (org[i] != payload[i]) {
                        changes[p]++;
                }
        }
}

void inject_message(unsigned char *dest, size_t dest_size, const unsigned char *msg, size_t msg_dim,
                    size_t step)
{
        size_t char_index = 0;
        size_t bit_index = 0;
        size_t dest_index = 0;

        for (size_t index = 0; index < msg_dim * QWORD; index += step) {
                if (dest_index > dest_size - 1) {
                        printf("[ERROR] - inject_message - Input file is too large, must be at most %ld\n"
                               "[ERROR] - inject_message - Reached dest_index: %ld\n",
                               (dest_size / (QWORD / step)) - QWORD - 1, dest_index);
                        exit(1);
                }

                char_index = index / QWORD;
                bit_index = index % 8;
                unsigned char byte_to_inject = get_isolated_bits(msg[char_index], bit_index, step);
                unsigned char mask = ~((1 << step) - 1);
                dest[dest_index] &= mask;
                dest[dest_index] |= byte_to_inject;
                dest_index++;
        }
}

void inject_lsbi_message(unsigned char *dest, size_t dest_size, const unsigned char *msg,
                         size_t msg_dim, size_t header_size)
{
        size_t from = header_size;
        size_t nBits = 1;
        for (size_t i = 0; i < msg_dim; i++) {
                unsigned char currByte = msg[i];

                for (int bitIndex = 0; bitIndex < 8; bitIndex += nBits) {
                        if ((from + 1) % 3 != 0) {
                                int bitValue = (currByte >> (8 - nBits - bitIndex)) &
                                               ((1 << nBits) - 1);

                                dest[from] = (dest[from] & ~((1 << nBits) - 1)) |
                                             (unsigned char)bitValue;
                        } else {
                                bitIndex -= nBits;
                        }
                        from++;
                }
        }
}

unsigned char get_isolated_bits(unsigned char value, size_t index, size_t length)
{
        unsigned char shift = 8 - length - index;
        value >>= shift;
        value &= (1 << length) - 1;
        return value;
}

#endif
