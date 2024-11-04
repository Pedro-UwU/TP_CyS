#ifndef EXTRACT_H
#define EXTRACT_H
#include <argument_parser.h>
#include <stdint.h>
#include <stddef.h>

// Declaraciones de funciones principales
void handle_extraction(Args *args);
void handle_extraction_lsb(Args *args, uint8_t step);
void handle_lsbi_extraction(Args *args);

#endif /* EXTRACT_H */
