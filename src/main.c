#ifndef MAIN
#define MAIN
#include <argument_parser.h>
#include <files.h>

int main(int argc, char *argv[]) {
  // Args* args = parse_arguments(argc, argv);
  BmpFile* bmpFile = get_bmp_file("./assets/red.bmp");
  describe_bmp(bmpFile);
  free_bmp_file(bmpFile);
}

#endif

