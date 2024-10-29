#ifndef ARGUMENT_PARSER
#define ARGUMENT_PARSER
#include <argument_parser.h>
#include <stdlib.h>
#include <string.h>
#include <def.h>

void init_args(Args* args) {
  args->embed = TRUE;
}

Args* parse_arguments(int argc, char* argv[]) {
  Args* args = malloc(sizeof(Args));
  init_args(args);

  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "-embed") == 0) {
      args->embed = TRUE;
    } else if (strcmp(argv[i], "-extract") == 0) {
      args->embed = FALSE;
    }
  }

  return args;
}

#endif
