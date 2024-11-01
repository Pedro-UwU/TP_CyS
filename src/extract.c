#ifndef EXTRACT
#define EXTRACT
#include <argument_parser.h>
#include <bmp_files.h>
#include <def.h>
#include <extract.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t get_message_lenght(uint8_t *payload, uint8_t bits);
void handle_extraction_lsb(Args *args, uint8_t step);
char *extract_message(uint8_t *payload, uint32_t message_size, uint8_t step);
char *extract_payload_extension(uint8_t *payload, uint32_t message_size,
                                uint8_t step);
void save_extracted_file(Args* args, char* payload, uint32_t payload_length, char* extension);

void handle_extraction(Args *args) {
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

void handle_extraction_lsb(Args *args, uint8_t step) {
  BmpFile *carrier = args->carrier;
  uint8_t *payload = carrier->payload;
  // size_t payload_length = carrier->info_header->sizeImage;
  uint32_t message_size = get_message_lenght(payload, step);
  char *message = extract_message(payload, message_size, step);
  char *extension = extract_payload_extension(payload, message_size, step);
  save_extracted_file(args, (char*)message, message_size, extension);
  free(message); // TO DELETE
  free(extension);
}

uint32_t get_message_lenght(uint8_t *payload, uint8_t step) {
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

char extract_char(uint8_t *payload, uint32_t index, uint8_t step) {
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

char *extract_message(uint8_t *payload, uint32_t message_size, uint8_t step) {
  char *message = malloc(message_size * sizeof(char) + 1);
  if (message == NULL) {
    printf(
        "[ERROR] - extract_message - Couldn't allocate memory for message\n");
    exit(1);
  }
  message[message_size] = '\0';
  for (uint32_t i = 0; i < message_size; i++) {
    char c = extract_char(
        payload, BITS_FOR_SIZE / step + i * sizeof(char) * 8 / step, step);
    message[i] = c;
  }
  return message;
}

char *extract_payload_extension(uint8_t *payload, uint32_t message_size,
                                uint8_t step) {
  char *extension = malloc(MAX_EXTENSION_LENGTH * sizeof(char));
  memset(extension, 0xFF, MAX_EXTENSION_LENGTH);
  if (extension == NULL) {
    printf("[ERROR] - extract_payload_extension - Couldn't allocate memory for "
           "extension\n");
    exit(1);
  }
  uint32_t i = 0;
  char last_read = 0xFF;
  for (i = 0; last_read != '\0'; i++) {
    char c = extract_char(payload,
                          BITS_FOR_SIZE / step +
                              (i + message_size) * sizeof(char) * 8 / step,
                          step);
    extension[i] = c;
    last_read = c;
  }
  char *to_return = malloc(i * sizeof(char));
  memcpy(to_return, extension, i);
  free(extension);
  return to_return;
}

void save_extracted_file(Args* args, char* payload, uint32_t payload_length, char* extension) {
  if (args == NULL || payload == NULL || extension == NULL) {
      printf("[ERROR] - save_extracted_file - Trying to save file with NULL arguments\n");
      exit(1);
  }
  printf("Length: %u\n", payload_length);
  char* path = args->out;
  char* full_path = malloc((strlen(path) + strlen(extension) + 1) * sizeof(char));
  strcpy(full_path, path);
  strcat(full_path, extension);
  FILE* output_file = fopen(full_path, "w");
  if (output_file == NULL) {
      printf("[ERROR] - save_extracted_file - Couldn't create file %s\n", full_path);
      exit(1);
  }
  printf("Output file %p\n", output_file);
  if (fwrite(payload, sizeof(char), payload_length, output_file) != payload_length) {
      printf("[ERROR] - save_extracted_file - Couldn't write to output file\n");
      exit(1);
  }
  printf("File created\n");
  fclose(output_file);
  free(full_path);
}


#endif
