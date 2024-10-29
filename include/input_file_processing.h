#ifndef LSB_H
#define LSB_H
#include <stddef.h>
#include <stdint.h>
typedef struct {
  uint32_t payload_size;
  unsigned char* payload;
  char* extension;
} InputData;

InputData* extract_lsb_data_from_file(char* path);
void free_lsb_data(InputData* data);
void describe_lsb_data(const InputData* data);
unsigned char *generate_unencrypted_payload(InputData *data, size_t* dim);

#endif
