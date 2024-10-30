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
  bool instruction = FALSE;
  bool carrier = FALSE;
  bool output = FALSE;
  bool steg = FALSE;
  bool input = FALSE;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-embed") == 0) {
      args->embed = TRUE;
      instruction = TRUE;
    } else if (strcmp(argv[i], "-extract") == 0) {
      args->embed = FALSE;
      instruction = TRUE;
    } else if (strcmp(argv[i], "-p") == 0) {
      if (i + 1 >= argc) {
        printf("[ERROR] - Must provide a valid carrier file\n");
        exit(1);
      }
      args->carrier = get_bmp_file(argv[++i]);
      carrier = TRUE;
    } else if (strcmp(argv[i], "-in") == 0) {
      if (i + 1 >= argc) {
        printf("[ERROR] - Must provide a valid input file\n");
        exit(1);
      }
      args->in_file = extract_lsb_data_from_file(argv[++i]);
      input = TRUE;
    } else if (strcmp(argv[i], "-out") == 0) {
      if (i + 1 >= argc) {
        printf("[ERROR] - Must provide a valid input file\n");
        exit(1);
      }
      char *args_output_file = argv[++i];
      char *output_file = malloc(strlen(args_output_file) + 1);
      strcpy(output_file, args_output_file);
      printf("OUTPUT: %s\n", output_file);
      args->out = output_file;
      output = TRUE;
    } else if (strcmp(argv[i], "-steg") == 0) {
      if (i + 1 >= argc) {
        printf("[ERROR] - Must provide a LSB method\n");
        exit(1);
      }
      char* lsb_arg = argv[++i];
      if (strcmp("LSB1", lsb_arg) == 0) {
        args->lsb_type = LSB1;
      } else if (strcmp("LSB4", lsb_arg) == 0) {
        args->lsb_type = LSB4; 
      } else if (strcmp("LSBI", lsb_arg) == 0) {
        args->lsb_type = LSBI; 
      } else {
        printf("[ERROR] - LSB method must be 'LSB1' or 'LSB4' or 'LSBI'\n");
        exit(1);
      }
      steg = TRUE;
    }
  }
  if (instruction == FALSE) {
    printf("[ERROR] - Either '-embed' or '-extract' flags must be present\n");
    exit(1);
  }
  if (!input && args->embed) {
    printf("[ERROR] - If embeding, an input file must be provided with '-in "
           "<file>'\n");
    exit(1);
  }
  if (!carrier) {
    printf("[ERROR] - A carrier file must be provided with '-p <file>'\n");
    exit(1);
  }
  if (!output) {
    printf("[ERROR] - An output file must be provided with '-out <file>'\n");
    exit(1);
  }
  if (!steg) {
    printf("[ERROR] - A steganography method must be selected with '-steg "
           "<LSB1 | LSB4 | LSBI>\n");
    exit(1);
  }

  return args;
}

#endif
