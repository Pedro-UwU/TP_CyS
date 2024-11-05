#ifndef FILES_H
#define FILES_H
#include <stdint.h>
#include <stdio.h>

#pragma pack(push, 1)
typedef struct {
        uint16_t type; // Letters "BM"
        uint32_t size;
        uint32_t reserved;
        uint32_t off_bits;
} BmpHeader;

typedef struct {
        uint32_t header_size;
        uint32_t width;
        uint32_t height;
        uint16_t planes;
        uint16_t bitCount;
        uint32_t compression;
        uint32_t sizeImage;
        uint32_t xPelsPerMeter;
        uint32_t yPelsPerMeter;
        uint32_t clrUsed;
        uint32_t clrImportant;
} BmpInfoHeader;
#pragma pack(pop)

typedef struct {
        FILE *file;
        BmpHeader *header;
        BmpInfoHeader *info_header;
        uint8_t *payload;
        uint32_t size;

} BmpFile;

BmpFile *get_bmp_file(const char *path);
void describe_bmp(const BmpFile *bmp);
void free_bmp_file(BmpFile *bmp_file);
void save_bmp(BmpFile *bmp, char *path);
unsigned char *clone_bmp_payload(BmpFile *bmp);

#endif
