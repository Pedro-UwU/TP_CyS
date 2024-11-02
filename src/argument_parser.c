#ifndef ARGUMENT_PARSER
#define ARGUMENT_PARSER
#include <bmp_files.h>
#include <argument_parser.h>
#include <input_file_processing.h>
#include <def.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_args(Args *args)
{
        args->action = ACTION_EMBED;
        args->carrier = NULL;
        args->in_file = NULL;
        args->out = NULL;
        args->encryption.algorithm = EncryptAlgo_NONE;
        args->encryption.mode = EncryptMode_NONE;
        args->encryption.password = NULL;
}

void set_encryption_algorithm(const char *algorithm, Args *args)
{
        if (strcmp(algorithm, "aes128") == 0) {
                args->encryption.algorithm = EncryptAlgo_AES128;
        } else if (strcmp(algorithm, "aes192") == 0) {
                args->encryption.algorithm = EncryptAlgo_AES192;
        } else if (strcmp(algorithm, "aes256") == 0) {
                args->encryption.algorithm = EncryptAlgo_AES256;
        } else if (strcmp(algorithm, "3des") == 0) {
                args->encryption.algorithm = EncryptAlgo_3DES;
        } else {
                printf("[ERROR] - Encryption Algorithm be one of 'aes128', 'aes192', 'aes256', '3des'\n");
                exit(1);
        }

        // Defaults
        if (args->encryption.mode == EncryptMode_NONE) {
                args->encryption.mode = EncryptMode_CBC;
        }
}

void set_encryption_mode(const char *mode, Args *args)
{
        if (strcmp(mode, "ecb") == 0) {
                args->encryption.mode = EncryptMode_ECB;
        } else if (strcmp(mode, "cfb") == 0) {
                args->encryption.mode = EncryptMode_CFB;
        } else if (strcmp(mode, "ofb") == 0) {
                args->encryption.mode = EncryptMode_OFB;
        } else if (strcmp(mode, "cbc") == 0) {
                args->encryption.mode = EncryptMode_CBC;
        } else {
                printf("[ERROR] - Encryption Mode be one of 'ecb', 'cfb', 'ofb', 'cbc'\n");
                exit(1);
        }

        // Defaults
        if (args->encryption.algorithm == EncryptAlgo_NONE) {
                args->encryption.algorithm = EncryptAlgo_AES128;
        }
}

void set_encryption_password(const char *password, Args *args)
{
        args->encryption.password = password;

        // Defaults
        if (args->encryption.algorithm == EncryptAlgo_NONE) {
                args->encryption.algorithm = EncryptAlgo_AES128;
        }

        if (args->encryption.mode == EncryptMode_NONE) {
                args->encryption.mode = EncryptMode_CBC;
        }
}

Args *parse_arguments(int argc, char *argv[])
{
        Args *args = malloc(sizeof(Args));
        init_args(args);
        bool instruction = FALSE;
        bool carrier = FALSE;
        bool output = FALSE;
        bool steg = FALSE;
        bool input = FALSE;

        for (int i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-embed") == 0) {
                        args->action = ACTION_EMBED;
                        instruction = TRUE;
                } else if (strcmp(argv[i], "-extract") == 0) {
                        args->action = ACTION_EXTRACT;
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
                        char *output_file = malloc(strlen(args_output_file) * sizeof(char) + 1);
                        strcpy(output_file, args_output_file);
                        args->out = output_file;
                        output = TRUE;
                } else if (strcmp(argv[i], "-steg") == 0) {
                        if (i + 1 >= argc) {
                                printf("[ERROR] - Must provide a LSB method\n");
                                exit(1);
                        }
                        char *lsb_arg = argv[++i];
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
                } else if (strcmp(argv[i], "-a") == 0) {
                        /* Encryption Algorithm */
                        set_encryption_algorithm(argv[++i], args);
                } else if (strcmp(argv[i], "-m") == 0) {
                        /* Encryption Mode */
                        set_encryption_mode(argv[++i], args);
                } else if (strcmp(argv[i], "-pass") == 0) {
                        /* Encryption Password */
                        set_encryption_password(argv[++i], args);
                }
        }
        if (instruction == FALSE) {
                printf("[ERROR] - Either '-embed' or '-extract' flags must be present\n");
                exit(1);
        }
        if (!input && args->action == ACTION_EMBED) {
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

        /* 
         * Son válidas en cambio las siguientes opciones:
         *         - Indicar algoritmo y password pero no modo: Se asume CBC por default.
         *         - Indicar modo y password pero no algoritmo: Se asume aes128 por default.
         *         - Indicar sólo password: Se asume algoritmo aes128 en modo CBC por default.
         */
        if (args->encryption.mode != EncryptMode_NONE && args->encryption.password == NULL) {
                printf("[ERROR] - A password must be provided alongside the encryption mode.\n");
                exit(1);
        }
        if (args->encryption.algorithm != EncryptAlgo_NONE && args->encryption.password == NULL) {
                printf("[ERROR] - A password must be provided alongside the encryption algorithm.\n");
                exit(1);
        }

        return args;
}

void free_args(Args *args)
{
        if (args == NULL) {
                return;
        }

        if (args->carrier != NULL) {
                free_bmp_file(args->carrier);
        }
        if (args->in_file != NULL) {
                free_lsb_data(args->in_file);
        }
        if (args->out != NULL) {
                free(args->out);
        }
        free(args);
}

#endif
