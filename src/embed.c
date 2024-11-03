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
void inject_message(unsigned char *dest, const unsigned char *msg, size_t msg_dim, size_t step);
void handle_lsb1(Args *args);
void handle_lsb4(Args *args);
// void handle_lsbI(Args* args);

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
        // case LSBI: handle_lsbI(args); break;
        default:
                break;
        }
}

void handle_lsb1(Args *args)
{
        InputData *input_data = args->in_file;
        BmpFile *bmp = args->carrier;
        unsigned char *payload;
        // TODO: Add encrypted
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

        if (dim * sizeof(char) > bmp->info_header->sizeImage) {
                printf("[ERROR] - handle_embedding - Input file is too large, must be at most %d\n",
                       bmp->info_header->sizeImage);
                exit(1);
        }

        inject_message(bmp->payload, payload, dim, 1);
        save_bmp(bmp, args->out);
        free(payload);
}

void handle_lsb4(Args *args)
{
        InputData *input_data = args->in_file;
        BmpFile *bmp = args->carrier;
        unsigned char *payload;
        // TODO: Add encrypted
        size_t dim = 0;
        payload = generate_unencrypted_payload(input_data, &dim);
        if (dim * sizeof(char) / 4 > bmp->info_header->sizeImage) {
                printf("[ERROR] - handle_embedding - Input file is too large, must be at most %d\n",
                       bmp->info_header->sizeImage);
                exit(1);
        }

        inject_message(bmp->payload, payload, dim, 4);
        free(payload);
        save_bmp(bmp, args->out);
}

// Step must be a divisor of 8
void inject_message(unsigned char *dest, const unsigned char *msg, size_t msg_dim, size_t step)
{
        size_t char_index = 0;
        size_t bit_index = 0;
        size_t dest_index = 0;

        for (size_t index = 0; index < msg_dim * 8; index += step) {
                char_index = index / 8;
                bit_index = index % 8;
                unsigned char byte_to_inject = get_isolated_bits(msg[char_index], bit_index, step);
                unsigned char mask = ~((1 << step) - 1);
                dest[dest_index] &= mask;
                dest[dest_index] |= byte_to_inject;
                dest_index++;
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
