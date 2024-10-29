#ifndef ARGUMENT_PARSER
#define ARGUMENT_PARSER
#include "bmp_files.h"
#include <argument_parser.h>
#include <def.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_args(Args *args) {
  args->embed = TRUE;
  args->carrier = NULL;
  args->in_file = NULL;
  args->out = NULL;
}

Args *parse_arguments(int argc, char *argv[]) {
  Args *args = malloc(sizeof(Args));
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
    } else if (strcmp(argv[i], "-out") == 0) {
      if (i + 1 >= argc) {
        printf("[ERROR] - Must provide a valid input file\n");
        exit(1);
      }
      char* args_output_file = argv[++i];
      char* output_file = malloc(strlen(args_output_file) + 1);
      strcpy(output_file, args_output_file);
      printf("OUTPUT: %s\n", output_file);
      args->out = output_file;
    }
  }

  return args;
}

#endif
