#ifndef LSB
#define LSB
#include "argument_parser.h"
#include <def.h>
#include <input_file_processing.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void extract_extension(InputData *data, char *path);
void extract_size(InputData *data, FILE *fp);
void extract_payload(InputData *data, FILE *fp);

InputData *extract_lsb_data_from_file(char *path) {
  InputData *data = malloc(sizeof(InputData));
  if (data == NULL) {
    printf("[ERROR] - extract_lsb_data_from_file - Couldn't allocate memory "
           "for InputData\n");
    exit(1);
  }
  extract_extension(data, path);
  FILE *in_file = fopen(path, "r");
  if (in_file == NULL) {
    printf("[ERROR] - extract_lsb_data_from_file - Couldn't open input file\n");
    exit(1);
  }
  extract_size(data, in_file);
  extract_payload(data, in_file);
  fclose(in_file);
  return data;
}

void free_lsb_data(InputData *data) {
  if (data == NULL) {
    return;
  }

  if (data->extension != NULL) {
    free(data->extension);
  }
  if (data->payload != NULL) {
    free(data->payload);
  }
  free(data);
}

void describe_lsb_data(const InputData *data) {
  if (data == NULL) {
    printf("InputData is NULL.\n");
    return;
  }

  printf("InputData Description:\n");
  printf("Payload Size: %u bytes\n", data->payload_size);
  printf("Extension: %s\n", data->extension ? data->extension : "(none)");

  if (data->payload && data->payload_size > 0) {
    printf("Payload (first 50 bytes or less): ");
    for (uint32_t i = 0; i < data->payload_size && i < 50; ++i) {
      printf("%02X ", data->payload[i]);
    }
    printf("\n");
  } else {
    printf("Payload is empty or NULL.\n");
  }
}

void init_lsb_data(InputData *data) {
  data->payload = NULL;
  data->extension = NULL;
  data->payload_size = 0;
}

void extract_extension(InputData *data, char *path) {
  size_t length = strlen(path);
  size_t last_dot_index = -1;
  bool found = FALSE;
  for (size_t i = 0; i < length; i++) {
    if (path[i] == '.') {
      last_dot_index = i;
      found = TRUE;
    }
  }
  if (!found) {
    printf("[ERROR] - extract_extension - File %s doesn't have an extension\n",
           path);
    exit(1);
  }
  size_t output_length = length - last_dot_index + 1;
  char *extension = malloc(output_length * sizeof(char));
  memset(extension, 0, output_length);
  strcpy(extension, &path[last_dot_index]);
  data->extension = extension;
}

void extract_size(InputData *data, FILE *fp) {
  fseek(fp, 0L, SEEK_END);
  uint32_t size = ftell(fp);
  fseek(fp, 0L, SEEK_SET);
  data->payload_size = size;
}

void extract_payload(InputData *data, FILE *fp) {
  data->payload = malloc(data->payload_size * sizeof(char));
  size_t read = fread(data->payload, 1, data->payload_size, fp);
  if (read != data->payload_size) {
    printf("[ERROR] - extract_payload - Couldn't read input file payload. "
           "Bytes read %lu\n",
           read);
    exit(1);
  }
}

unsigned char *generate_unencrypted_payload(InputData *data, size_t* dim) {
  size_t extension_len = strlen(data->extension) + 1;
  size_t size = 4 + data->payload_size + extension_len * sizeof(char);
  unsigned char *result = malloc(size + 1);
  uint32_t result_size = data->payload_size;
  // Save in big endian
  result[0] = (result_size >> 24) & 0xFF;
  result[1] = (result_size >> 16) & 0xFF;
  result[2] = (result_size >> 8) & 0xFF;
  result[3] = result_size & 0xFF;
  memcpy(result + 4, data->payload, data->payload_size);
  memcpy(result + 4 + data->payload_size, data->extension, extension_len);
  printf("Extension writen: %s at %u\n", &result[4 + data->payload_size], data->payload_size);
  result[size] = '\0';
  *dim = size;
  return result;
}


#endif
