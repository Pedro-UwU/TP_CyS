#ifndef MAIN
#define MAIN
#include <def.h>
#include <argument_parser.h>
#include <bmp_files.h>
#include <input_file_processing.h>
#include <embed.h>
#include <extract.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  Args* args = parse_arguments(argc, argv);

  if (args->embed) {
    printf("EMBEDDING\n");
    handle_embedding(args);
  } else {
    printf("EXTRACTING\n");
    handle_extraction(args);
  }
  free_args(args);
}

#endif

