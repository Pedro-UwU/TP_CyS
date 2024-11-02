#ifndef MAIN
#define MAIN
#include <def.h>
#include <bmp_files.h>
#include <input_file_processing.h>
#include <embed.h>
#include <extract.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
        Args *args = parse_arguments(argc, argv);

        if (args->action == ACTION_EMBED) {
                printf("EMBEDDING\n");
                handle_embedding(args);
        } else if (args->action == ACTION_EXTRACT) {
                printf("EXTRACTING\n");
                handle_extraction(args);
        } else {
                printf("Invalid action detected. Shame to the programmer\n");
        }
        free_args(args);
}

#endif
