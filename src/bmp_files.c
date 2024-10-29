#ifndef FILES
#define FILES
#include <bmp_files.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_headers(BmpFile *bmp);
void read_bmp(FILE *fp, BmpFile *bmp);
void check_image_size(BmpFile* bmp);

void init_bmp_files(BmpFile *bmp_file) {
  bmp_file->file = NULL;
  bmp_file->header = NULL;
  bmp_file->info_header = NULL;
  bmp_file->payload = NULL;
  bmp_file->size = 0;
}

BmpFile *get_bmp_file(const char *path) {
  BmpFile *bmp_file = malloc(sizeof(BmpFile));
  init_bmp_files(bmp_file);

  FILE *file = fopen(path, "r");
  if (file == NULL) {
    printf("[ERROR] - get_bmp_file - Couldn't open file %s\n", path);
  }
  read_bmp(file, bmp_file);

  return bmp_file;
}

void free_bmp_file(BmpFile *bmp_file) {
  if (bmp_file == NULL)
    return;
  if (bmp_file->file != NULL) {
    free(bmp_file->file);
  }
  if (bmp_file->header != NULL) {
    free(bmp_file->header);
  }

  if (bmp_file->info_header != NULL) {
    free(bmp_file->info_header);
  }
  if (bmp_file->payload != NULL) {
    free(bmp_file->payload);
  }
  free(bmp_file);
}

void describe_bmp(const BmpFile *bmp) {
  if (!bmp || !bmp->header || !bmp->info_header) {
    printf("Invalid BMP file structure\n");
    return;
  }

  printf("BMP File Description:\n");
  printf("-------------------\n");

  // File Header Information
  printf("File Header:\n");
  printf("  Type: %c%c\n", (char)(bmp->header->type & 0xFF),
         (char)((bmp->header->type >> 8) & 0xFF));
  printf("  File Size: %d bytes\n", bmp->header->size);
  printf("  Data Offset: %u bytes\n", bmp->header->off_bits);

  // Info Header Information
  printf("\nInfo Header:\n");
  printf("  Header Size: %u bytes\n", bmp->info_header->header_size);
  printf("  Image Dimensions: %ux%u pixels\n", bmp->info_header->width,
         bmp->info_header->height);
  printf("  Color Planes: %u\n", bmp->info_header->planes);
  printf("  Bits per Pixel: %u\n", bmp->info_header->bitCount);

  // Compression Information
  printf("  Compression: ");
  switch (bmp->info_header->compression) {
  case 0:
    printf("None (BI_RGB)\n");
    break;
  case 1:
    printf("RLE 8-bit/pixel (BI_RLE8)\n");
    break;
  case 2:
    printf("RLE 4-bit/pixel (BI_RLE4)\n");
    break;
  case 3:
    printf("Bitfields (BI_BITFIELDS)\n");
    break;
  default:
    printf("Unknown (%u)\n", bmp->info_header->compression);
  }

  printf("  Image Size: %u bytes\n", bmp->info_header->sizeImage);
  printf("  Resolution: %u x %u pixels/meter\n",
         bmp->info_header->xPelsPerMeter, bmp->info_header->yPelsPerMeter);

  // Color Information
  printf("  Colors Used: %u\n", bmp->info_header->clrUsed);
  printf("  Important Colors: %u\n", bmp->info_header->clrImportant);

  // Additional Information
  printf("\nPayload Size: %u bytes\n", bmp->size);
}

void read_bmp(FILE *fp, BmpFile *bmp) {
  if (fp == NULL || bmp == NULL) {
    printf("[ERROR] read_bmp");
    exit(1);
  }
  init_headers(bmp);
  if (fread(bmp->header, sizeof(BmpHeader), 1, fp) != 1) {
    printf("[ERROR] - read_bmp - Couldn't read bmp header\n");
    exit(1);
  }
  if (fread(bmp->info_header, sizeof(BmpInfoHeader), 1, fp) != 1) {
    printf("[ERROR] - read_bmp - Couldn't read bmp info header\n");
    exit(1);
  }
  uint32_t offset = bmp->header->off_bits;
  check_image_size(bmp);
  bmp->payload = malloc(bmp->info_header->sizeImage * sizeof(uint8_t));
  if (bmp->payload == NULL) {
    printf("[ERROR] - read_bmp - Couldn't allocate memory for payload\n");
    exit(1);
  }
  if (fseek(fp, offset, SEEK_SET) != 0) {
    printf("[ERROR] - read_bmp - Couldn't offset file reading\n");
    exit(1);
  }

  if (fread(bmp->payload, bmp->info_header->sizeImage, 1, fp) != 1) {
    printf("[ERROR] - read_bmp - Couldn't read image payload\n");
    exit(1);
  }
}

void init_headers(BmpFile *bmp) {
  bmp->header = malloc(sizeof(BmpHeader));
  bmp->info_header = malloc(sizeof(BmpInfoHeader));
  if (bmp->header == NULL || bmp->info_header == NULL) {
    printf("[ERROR] - init_header - Couldn't allocate memory\n");
    exit(1);
  }
  bmp->header->size = 0;
  bmp->header->type = 0;
  bmp->header->reserved = 0;
  bmp->header->off_bits = 0;
}

void check_image_size(BmpFile* bmp) {
  if (bmp->info_header->sizeImage > 0) {
    return;
  }
  uint32_t size = bmp->header->size - bmp->header->off_bits;
  bmp->info_header->sizeImage = size;
  bmp->size = size;
}

#endif
