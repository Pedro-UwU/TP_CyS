#ifndef MAIN
#define MAIN
#include <def.h>
#include <argument_parser.h>
#include <bmp_files.h>
#include <input_file_processing.h>
#include <embed.h>

int main(int argc, char *argv[]) {
  Args* args = parse_arguments(argc, argv);
  BmpFile* bmpFile = args->carrier;
  handle_embedding(args, LSB1);
  free_bmp_file(bmpFile);
  free_lsb_data(args->in_file);
}

#endif

