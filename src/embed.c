#ifndef EMBED
#define EMBED
#include <argument_parser.h>
#include <bmp_files.h>
#include <def.h>
#include <input_file_processing.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

unsigned char get_isolated_bits(unsigned char value, size_t index,
                                size_t length);
void inject_message(unsigned char *dest, const unsigned char *msg,
                    size_t msg_dim, size_t step);
void handle_lsb1(Args *args);
void handle_lsb4(Args *args);
// void handle_lsbI(Args* args);

void handle_embedding(Args *args, LsbType lsb_type) {
  switch (lsb_type) {
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

void handle_lsb1(Args *args) {
  InputData *input_data = args->in_file;
  BmpFile *bmp = args->carrier;
  unsigned char *payload;
  // TODO: Add unencrypted
  size_t dim = 0;
  payload = generate_unencrypted_payload(input_data, &dim);
  if (dim * sizeof(char) > bmp->info_header->sizeImage) {
    printf("[ERROR] - handle_embedding - Input file is too large\n");
    exit(1);
  }

  inject_message(bmp->payload, payload, dim, 2);
  save_bmp(bmp, args->out);
}

void handle_lsb4(Args *args) {
  InputData *input_data = args->in_file;
  BmpFile *bmp = args->carrier;
  unsigned char *payload;
  // TODO: Add unencrypted
  size_t dim = 0;
  payload = generate_unencrypted_payload(input_data, &dim);
  if (dim * sizeof(char) / 4 > bmp->info_header->sizeImage) {
    printf("[ERROR] - handle_embedding - Input file is too large\n");
    exit(1);
  }

  inject_message(bmp->payload, payload, dim, 4);
  save_bmp(bmp, args->out);
}

// Step must be a divisor of 8
void inject_message(unsigned char *dest, const unsigned char *msg,
                    size_t msg_dim, size_t step) {
  size_t char_index = 0;
  size_t bit_index = 0;
  size_t dest_index = 0;

  printf("1- char %02X\n", msg[0]);
  printf("2- char %02X\n", msg[1]);
  printf("3- char %02X\n", msg[2]);
  printf("4- char %02X\n", msg[3]);
  printf("Message len = %lu\n", msg_dim);

  for (size_t index = 0; index < msg_dim * 8; index += step) {
    char_index = index / 8;
    bit_index = index % 8;
    unsigned char byte_to_inject =
        get_isolated_bits(msg[char_index], bit_index, step);
    unsigned char mask = ~((1 << step) - 1);
    dest[dest_index] &= mask;
    dest[dest_index] |= byte_to_inject;
    dest_index++;
  }
}

unsigned char get_isolated_bits(unsigned char value, size_t index,
                                size_t length) {
  unsigned char shift = 8 - length - index;
  value >>= shift;
  value &= (1 << length) - 1;
  return value;
}

#endif