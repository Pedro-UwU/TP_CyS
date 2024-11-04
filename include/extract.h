#ifndef EXTRACT_H
#define EXTRACT_H
#include <argument_parser.h>
#include <stdint.h>
#include <stddef.h>

// Declaraciones de funciones principales
void handle_extraction(Args *args);
void handle_extraction_lsb(Args *args, uint8_t step);
void handle_lsbi_extraction(Args *args);

// Declaraciones de funciones auxiliares
uint32_t get_payload_message_lenght(uint8_t *payload, uint8_t step);
uint32_t get_message_lenght(uint8_t *message);
char extract_char(uint8_t *payload, uint32_t index, uint8_t step, size_t max_size);
char *extract_message(uint8_t *payload, uint32_t message_size, uint8_t step, size_t max_size);
char *extract_payload_extension(uint8_t *payload, uint32_t message_size, uint8_t step);
char *extract_message_extension(unsigned char *message, uint32_t message_size);
void save_extracted_file(Args *args, char *payload, uint32_t payload_length, char *extension);

#endif /* EXTRACT_H */