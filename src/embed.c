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

unsigned char get_isolated_bits(unsigned char value, size_t index, size_t length);
void inject_message(unsigned char *dest, size_t dest_size, const unsigned char *msg, size_t msg_dim,
                    size_t step);
void handle_lsb1(Args *args);
void handle_lsb4(Args *args);
void handle_lsbi(Args *args);

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

        // Verificar si hay espacio suficiente (considerando los 4 bytes extra para el pattern map)
        size_t required_bits = (input_data->payload_size + strlen(input_data->extension) + 5) * 8;
        if (required_bits > bmp->info_header->sizeImage * 3) {
                printf("[ERROR] - handle_lsbi - Input file is too large. Maximum capacity is %lu bytes\n",
                       (bmp->info_header->sizeImage * 3 / 8) - 5);
                exit(1);
        }

        // Preparar payload
        if (args->encryption.algorithm == EncryptAlgo_NONE) {
                payload = generate_unencrypted_payload(input_data, &dim);
        } else {
                size_t p_dim = 0;
                unsigned char *p_payload = generate_unencrypted_payload(input_data, &p_dim);
                payload = encrypt_payload(&args->encryption, p_payload, p_dim, &dim);
                free(p_payload);
        }

        // Crear copia del payload original para comparación
        unsigned char *original_payload = malloc(bmp->info_header->sizeImage);
        memcpy(original_payload, bmp->payload, bmp->info_header->sizeImage);

        // Aplicar LSB1 estándar primero
        inject_message(bmp->payload, bmp->info_header->sizeImage, payload, dim, 1);

        // Analizar cambios por patrón
        uint8_t pattern_map = 0;
        for (uint8_t pattern = 0; pattern < 4; pattern++) {
                if (check_pixel_changes(original_payload, bmp->payload, bmp->info_header->sizeImage,
                                        pattern)) {
                        pattern_map |= (1 << (3 - pattern));
                        invert_pattern_bits(bmp->payload, bmp->info_header->sizeImage, pattern);
                }
        }

        // Insertar el mapa de patrones al principio
        inject_pattern_map(bmp->payload, pattern_map);

        // Guardar resultado
        save_bmp(bmp, args->out);

        free(payload);
        free(original_payload);
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
                bit_index = index % QWORD;
                unsigned char byte_to_inject = get_isolated_bits(msg[char_index], bit_index, step);
                unsigned char mask = ~((1 << step) - 1);
                dest[dest_index] &= mask;
                dest[dest_index] |= byte_to_inject;
                dest_index++;
        }
}

unsigned char get_isolated_bits(unsigned char value, size_t index, size_t length)
{
        unsigned char shift = QWORD - length - index;
        value >>= shift;
        value &= (1 << length) - 1;
        return value;
}

#endif
