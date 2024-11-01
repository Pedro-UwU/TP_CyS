#ifndef EXTRACT
#define EXTRACT
#include <argument_parser.h>
#include <bmp_files.h>
#include <extract.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <def.h>

uint32_t get_message_lenght(uint8_t *payload, uint8_t bits);
void handle_extraction_lsb(Args *args, uint8_t step);
char *extract_message(uint8_t *payload, uint32_t message_size, uint8_t step);

void handle_extraction(Args *args)
{
        if (args == NULL) {
                printf("[ERROR] - extract_message_from_file - NULL argument struct\n");
                exit(1);
        }

        switch (args->lsb_type) {
        case LSB1:
                handle_extraction_lsb(args, 1);
                break;
        case LSB4:
                handle_extraction_lsb(args, 4);
                break;
        case LSBI:
                break;
        }
}

void handle_extraction_lsb(Args *args, uint8_t step)
{
        BmpFile *carrier = args->carrier;
        uint8_t *payload = carrier->payload;
        // size_t payload_length = carrier->info_header->sizeImage;
        uint32_t message_size = get_message_lenght(payload, step);
        char *message = extract_message(payload, message_size, step);
        free(message); // TO DELETE
}

uint32_t get_message_lenght(uint8_t *payload, uint8_t step)
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
        return 0;
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
        printf("Extracted: %s\n", message);
        return message;
}

#endif
