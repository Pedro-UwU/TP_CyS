#ifndef MAIN
#define MAIN
#include <argument_parser.h>
#include <bmp_files.h>

int main(int argc, char *argv[]) {
  Args* args = parse_arguments(argc, argv);
  BmpFile* bmpFile = args->carrier;
  describe_bmp(bmpFile);
  free_bmp_file(bmpFile);
}

#endif

