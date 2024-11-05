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
                         size_t msg_dim);

void handle_lsb1(Args *args);
void handle_lsb4(Args *args);
void handle_lsbi(Args *args);

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

        size_t dim = 0;

        unsigned char *payload;
        unsigned char *bmp_payload_copy = clone_bmp_payload(bmp);

        size_t pattern_count[N_LSBI_Pattern] = { 0 };
        size_t pattern_changes[N_LSBI_Pattern] = { 0 };

        if (args->encryption.algorithm == EncryptAlgo_NONE) {
                payload = generate_unencrypted_payload(input_data, &dim);
        } else {
                size_t p_dim = 0;
                unsigned char *p_payload;

                p_payload = generate_unencrypted_payload(input_data, &p_dim);
                payload = encrypt_payload(&args->encryption, p_payload, p_dim, &dim);
                free(p_payload);
        }

        inject_lsbi_message(bmp->payload, bmp->info_header->sizeImage, payload, dim);

        lsbi_calculate_patterns(bmp_payload_copy, bmp->payload, bmp->info_header->sizeImage,
                                pattern_count, pattern_changes);

        lsbi_apply_pattern_map_to_payload(bmp->payload, bmp->info_header->sizeImage, pattern_count,
                                          pattern_changes);

        save_bmp(bmp, args->out);

        free(payload);
        free(bmp_payload_copy);
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
                         size_t msg_dim)
{
        size_t index = N_LSBI_Pattern;
        for (size_t i = 0; i < msg_dim; i++) {
                unsigned char cbyte = msg[i];

                printf("index: %ld\n", index);

                for (int bit_index = 0; bit_index < 8; bit_index += 1) {
                        if (index > dest_size - 1) {
                                printf("[ERROR] - inject_lsbi_message - Input file is too large, must be at most %ld\n"
                                       "[ERROR] - inject_lsbi_message - Reached dest_index: %ld\n",
                                       (2 * dest_size / 3 / QWORD) - 4 - 6, index);
                                exit(1);
                        }

                        if ((index + 1) % 3 != 0) {
                                int byte_to_inject = get_isolated_bits(cbyte, bit_index, 1);
                                dest[index] &= ~((1 << 1) - 1);
                                dest[index] |= byte_to_inject;
                        } else {
                                bit_index -= 1;
                        }

                        index++;
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
