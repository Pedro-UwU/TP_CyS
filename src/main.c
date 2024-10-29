#ifndef MAIN
#define MAIN
#include "lsb.h"
#include <argument_parser.h>
#include <bmp_files.h>

int main(int argc, char *argv[]) {
  Args* args = parse_arguments(argc, argv);
  BmpFile* bmpFile = args->carrier;
  describe_lsb_data(args->in_file);
  free_bmp_file(bmpFile);
  free_lsb_data(args->in_file);
}

#endif

