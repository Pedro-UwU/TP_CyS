#ifndef LSB_H
#define LSB_H
#include <stdint.h>
typedef struct {
  uint32_t payload_size;
  unsigned char* payload;
  char* extension;
} LsbData;

LsbData* extract_lsb_data_from_file(char* path);
void free_lsb_data(LsbData* data);
void describe_lsb_data(const LsbData* data);
#endif
