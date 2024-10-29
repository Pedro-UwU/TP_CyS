#ifndef ARGUMENT_PARSER
#define ARGUMENT_PARSER
#include "bmp_files.h"
#include <argument_parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <def.h>
#include <lsb.h>

void init_args(Args* args) {
  args->embed = TRUE;
  args->carrier = NULL;
  args->in_file = NULL;
}

Args* parse_arguments(int argc, char* argv[]) {
  Args* args = malloc(sizeof(Args));
  init_args(args);

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-embed") == 0) {
      args->embed = TRUE;
    } else if (strcmp(argv[i], "-extract") == 0) {
      args->embed = FALSE;
    } else if (strcmp(argv[i], "-p") == 0) {
      if (i + 1 >= argc) {
        printf("[ERROR] - Must provide a valid carrier file\n");
        exit(1);
      }
      args->carrier = get_bmp_file(argv[++i]);
    } else if (strcmp(argv[i], "-in") == 0) {
      if (i + 1 >= argc) {
        printf("[ERROR] - Must provide a valid input file\n");
        exit(1);
      }
      args->in_file = extract_lsb_data_from_file(argv[++i]);
    }
  }

  return args;
}

#endif
