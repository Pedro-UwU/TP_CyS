#ifndef EXTRACT
#define EXTRACT
#include "encrypt.h"
#include <argument_parser.h>
#include <bmp_files.h>
#include <def.h>
#include <extract.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations
void handle_extraction_lsb(Args *args, uint8_t step);
void handle_lsbi_extraction(Args *args);
void extract_encrypted_msg(Args *args, uint8_t step);


void handle_extraction(Args *args)
{
    if (args == NULL) {
        printf("[ERROR] - extract_message_from_file - NULL argument struct\n");
        exit(1);
    }

    if (args->encryption.algorithm == EncryptAlgo_NONE) {
        switch (args->lsb_type) {
        case LSB1:
            handle_extraction_lsb(args, 1);
            break;
        case LSB4:
            handle_extraction_lsb(args, 4);
            break;
        case LSBI:
            handle_lsbi_extraction(args);
            break;
        }
    } else {
        switch (args->lsb_type) {
        case LSB1:
            extract_encrypted_msg(args, 1);
            break;
        case LSB4:
            extract_encrypted_msg(args, 4);
            break;
        case LSBI:
            handle_lsbi_extraction(args);
            break;
        }
    }
}

void extract_encrypted_msg(Args *args, uint8_t step)
{
    BmpFile *carrier = args->carrier;
    uint8_t *payload = carrier->payload;
    size_t max_size = carrier->info_header->sizeImage;

    uint32_t e_message_size = get_payload_message_lenght(payload, step);
    char *e_message = extract_message(payload, e_message_size, step, max_size);

    size_t d_size = 0;
    unsigned char *decrypted = decrypt_payload(&args->encryption, 
                                            (unsigned char *)e_message,
                                            e_message_size, 
                                            &d_size);

    uint32_t message_size = get_message_lenght(decrypted);
    unsigned char *message = decrypted + DWORD;
    char *extension = extract_message_extension(message, message_size);

    save_extracted_file(args, (char *)message, message_size, extension);

    free(decrypted);
    free(e_message);
}

void handle_extraction_lsb(Args *args, uint8_t step)
{
    BmpFile *carrier = args->carrier;
    uint8_t *payload = carrier->payload;
    size_t max_size = carrier->info_header->sizeImage;

    uint32_t message_size = get_payload_message_lenght(payload, step);
    char *message = extract_message(payload, message_size, step, max_size);
    char *extension = extract_payload_extension(payload, message_size, step);
    save_extracted_file(args, message, message_size, extension);
    free(message);
    free(extension);
}

char extract_char(uint8_t *payload, uint32_t index, uint8_t step, size_t max_size)
{
    uint32_t total_steps = sizeof(char) * 8 / step;
    uint8_t c = 0;
    
    if (index + total_steps > max_size) {
        printf("[ERROR] - extract_char - Index out of bounds\n");
        exit(1);
    }

    for (uint32_t i = 0; i < total_steps; i++) {
        uint8_t bits = payload[index + i];
        bits &= ((1 << step) - 1);
        c <<= step;
        c |= bits;
    }
    return (char)c;
}

char *extract_message(uint8_t *payload, uint32_t message_size, uint8_t step, size_t max_size)
{
    char *message = malloc(message_size * sizeof(char) + 1);
    if (message == NULL) {
        printf("[ERROR] - extract_message - Couldn't allocate memory for message\n");
        exit(1);
    }
    message[message_size] = '\0';
    
    uint32_t bytes_per_char = 8 / step; // Cuántos bytes necesitamos por carácter
    
    // Verificar que hay espacio suficiente
    if (bytes_per_char * message_size > max_size) {
        printf("[ERROR] - extract_message - Message size exceeds payload bounds\n");
        free(message);
        exit(1);
    }

    for (uint32_t i = 0; i < message_size; i++) {
        uint8_t c = 0;
        for (uint32_t j = 0; j < 8; j++) {
            uint32_t byte_index = (i * 8 + j) / step;
            uint8_t bit = (payload[byte_index] & (1 << 0)) ? 1 : 0;
            c = (c << 1) | bit;
        }
        message[i] = c;
    }
    
    return message;
}

void handle_lsbi_extraction(Args *args) {
    BmpFile *carrier = args->carrier;
    size_t payload_size = carrier->info_header->sizeImage;
    
    printf("BMP Analysis:\n");
    printf("Image dimensions: %d x %d\n", carrier->info_header->width, carrier->info_header->height);
    printf("Payload size: %zu bytes\n", payload_size);
    
    uint8_t *payload_copy = malloc(payload_size);
    if (!payload_copy) {
        printf("[ERROR] - handle_lsbi_extraction - Failed to allocate memory\n");
        exit(1);
    }
    memcpy(payload_copy, carrier->payload, payload_size);

    // Debug: Mostrar los primeros bytes del payload
    printf("\nFirst 40 bytes of payload (raw):\n");
    for(int i = 0; i < 40; i++) {
        printf("%02X ", payload_copy[i]);
        if((i + 1) % 8 == 0) printf("\n");
    }

    // 1. Extraer pattern map (primeros 4 bytes)
    uint8_t pattern_map = 0;
    printf("\nPattern map extraction (first 4 bytes):\n");
    for(int i = 0; i < 4; i++) {
        uint8_t byte = payload_copy[i];
        uint8_t bit = byte & 0x01;
        pattern_map |= (bit << (3-i));
        printf("Byte %d: %02X (LSB: %d)\n", i, byte, bit);
    }
    
    printf("Final pattern map: %d%d%d%d\n",
           (pattern_map >> 3) & 1,
           (pattern_map >> 2) & 1,
           (pattern_map >> 1) & 1,
           pattern_map & 1);

    // 2. Extraer el tamaño (siguiente bloque de 32 bits)
    uint32_t message_size = 0;
    printf("\nSize extraction (next 32 bits):\n");
    for(int i = 0; i < 32; i++) {
        size_t byte_index = 4 + i;
        uint8_t current_byte = payload_copy[byte_index];
        uint8_t pattern = (current_byte >> 1) & 0x03;
        uint8_t bit = current_byte & 0x01;
        
        // Aplicar inversión según el pattern map
        if(pattern_map & (1 << (3-pattern))) {
            bit ^= 1;
        }
        
        printf("Bit %2d: Byte=%02X Pattern=%d LSB=%d (after inversion=%d)\n",
               i, current_byte, pattern, current_byte & 0x01, bit);
        
        message_size = (message_size << 1) | bit;
    }
    
    printf("\nExtracted size: %u (0x%08X) bytes\n", message_size, message_size);
    
    // Validar tamaño
    size_t max_allowed = (payload_size - 36) / 8;  // Cada byte puede almacenar 1 bit
    printf("Maximum allowed size: %zu bytes\n", max_allowed);

    if (message_size == 0 || message_size > max_allowed) {
        printf("[ERROR] - handle_lsbi_extraction - Invalid message size: %u (max allowed: %zu)\n",
               message_size, max_allowed);
        free(payload_copy);
        exit(1);
    }

    free(payload_copy);
}

uint32_t get_payload_message_lenght(uint8_t *payload, uint8_t step)
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
}

uint32_t get_message_lenght(uint8_t *message)
{
    uint32_t size = 0;
    size |= ((uint32_t)message[0] << 24);
    size |= ((uint32_t)message[1] << 16);
    size |= ((uint32_t)message[2] << 8);
    size |= ((uint32_t)message[3]);
    return size;
}

char *extract_payload_extension(uint8_t *payload, uint32_t message_size, uint8_t step)
{
    char *extension = malloc(MAX_EXTENSION_LENGTH * sizeof(char));
    if (extension == NULL) {
        printf("[ERROR] - extract_payload_extension - Couldn't allocate memory for extension\n");
        exit(1);
    }
    
    uint32_t i = 0;
    unsigned char last_read = 0xFF;
    uint32_t base_index = BITS_FOR_SIZE / step + message_size * sizeof(char) * 8 / step;
    
    for (i = 0; last_read != '\0' && i < MAX_EXTENSION_LENGTH - 1; i++) {
        last_read = extract_char(payload, base_index + i * sizeof(char) * 8 / step, step, 
                               message_size * sizeof(char) * 8 + MAX_EXTENSION_LENGTH * 8);
        extension[i] = last_read;
    }
    
    extension[i] = '\0';
    return extension;
}

char *extract_message_extension(unsigned char *message, uint32_t message_size)
{
    return (char *)(message + message_size);
}

void save_extracted_file(Args *args, char *payload, uint32_t payload_length, char *extension)
{
    if (args == NULL || payload == NULL || extension == NULL) {
        printf("[ERROR] - save_extracted_file - Trying to save file with NULL arguments\n");
        exit(1);
    }
    char *path = args->out;
    char *full_path = malloc((strlen(path) + strlen(extension) + 1) * sizeof(char));
    strcpy(full_path, path);
    strcat(full_path, extension);
    FILE *output_file = fopen(full_path, "w");
    if (output_file == NULL) {
        printf("[ERROR] - save_extracted_file - Couldn't create file %s\n", full_path);
        exit(1);
    }
    if (fwrite(payload, sizeof(char), payload_length, output_file) != payload_length) {
        printf("[ERROR] - save_extracted_file - Couldn't write to output file\n");
        exit(1);
    }
    printf("Created file: %s\n", full_path);
    fclose(output_file);
    free(full_path);
}

#endif /* EXTRACT */