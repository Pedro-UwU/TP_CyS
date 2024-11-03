#ifndef ENCRYPT
#define ENCRYPT

#include <argument_parser.h>
#include <bmp_files.h>
#include <def.h>
#include <embed.h>
#include <input_file_processing.h>
#include <encrypt.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <openssl/evp.h>

// Consigna
static const uint8_t *__SALT = 0x0000000000000000;

typedef enum { EncryptAction_DECRYPT = 0, EncryptAction_ENCRYPT = 1 } EncryptAction;

static const char *algorithm_name(EncryptionAlgorithm algorithm)
{
        switch (algorithm) {
        case EncryptAlgo_AES128:
                return "aes-128";
        case EncryptAlgo_AES192:
                return "aes-192";
        case EncryptAlgo_AES256:
                return "aes-256";
        case EncryptAlgo_3DES:
                return "des-ede3";
        case EncryptAlgo_NONE:
                break;
        }

        printf("[ERROR] - algorithm_name - Got NONE or invalid algorithm\n");
        return "";
}

static const char *mode_name(EncryptionMode mode)
{
        switch (mode) {
        case EncryptMode_ECB:
                return "ecb";
        case EncryptMode_CFB:
                return "cfb";
        case EncryptMode_OFB:
                return "ofb";
        case EncryptMode_CBC:
                return "cbc";
        case EncryptMode_NONE:
                break;
        }

        printf("[ERROR] - mode_name - Got NONE or invalid mode\n");
        return "";
}

static const EVP_CIPHER *get_cipher(EncryptionAlgorithm algorithm, EncryptionMode mode)
{
        const char *a = algorithm_name(algorithm);
        const char *m = mode_name(mode);

        char *c = calloc(strlen(a) + 1 + strlen(m) + 1, sizeof(char));
        strcpy(c, a);
        strcat(c, "-");
        strcat(c, m);

        const EVP_CIPHER *cipher = EVP_get_cipherbyname(c);
        free(c);
        return cipher;
}

static int encrypt_decrypt_msg(Encryption *encryption, EncryptAction action,
                               const unsigned char *msg, size_t msg_len, unsigned char *out,
                               size_t *size)
{
        int ret = 0;

        unsigned char *key_iv = NULL;
        unsigned char *key = NULL;
        int key_len = 0;
        unsigned char *iv = NULL;
        int iv_len = 0;

        int outlen = 0;
        int len = 0;

        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        if (ctx == NULL) {
                printf("[ERROR] - encrypt_decrypt_msg - Cipher context is NULL\n");
                goto end;
        }

        const EVP_CIPHER *cipher = get_cipher(encryption->algorithm, encryption->mode);
        if (cipher == NULL) {
                printf("[ERROR] - encrypt_decrypt_msg - Cipher is NULL\n");
                goto end;
        }

        key_len = EVP_CIPHER_key_length(cipher);
        iv_len = EVP_CIPHER_iv_length(cipher);
        key_iv = calloc(key_len + iv_len + 1, sizeof(unsigned char));
        if (key_iv == NULL) {
                printf("[ERROR] - encrypt_decrypt_msg - Could not allocate memory for key_iv\n");
                goto end;
        }

        if (0 == PKCS5_PBKDF2_HMAC(encryption->password, -1, (unsigned char *)__SALT, 0, 10000,
                                   EVP_sha256(), key_len + iv_len, key_iv)) {
                printf("[ERROR] - encrypt_decrypt_msg - PKCS5_PBKDF2_HMAC error\n");
                goto end;
        }

        key = calloc(key_len, sizeof(unsigned char));
        if (key == NULL) {
                printf("[ERROR] - encrypt_decrypt_msg - Could not allocate memory for key\n");
                goto end;
        }

        iv = calloc(iv_len, sizeof(unsigned char));
        if (iv == NULL) {
                printf("[ERROR] - encrypt_decrypt_msg - Could not allocate memory for iv\n");
                goto end;
        }

        memcpy(key, key_iv, key_len);
        memcpy(iv, key_iv + key_len, iv_len);

        // Perform encryption / decryption
        if (0 == EVP_CipherInit_ex(ctx, cipher, NULL, key, iv, action)) {
                printf("[ERROR] - encrypt_decrypt_msg - CipherInit_ex error\n");
                goto end;
        }

        if (0 == EVP_CipherUpdate(ctx, out, &outlen, msg, msg_len)) {
                printf("[ERROR] - encrypt_decrypt_msg - CipherUpdate error\n");
                goto end;
        }

        if (0 == EVP_CipherFinal_ex(ctx, out + outlen, &len)) {
                printf("[ERROR] - encrypt_decrypt_msg - CipherFinal_ex error\n");
                goto end;
        }

        *size = outlen + len;
        ret = 1;

end:
        free(iv);
        free(key);
        free(key_iv);
        EVP_CIPHER_CTX_free(ctx);

        return ret;
}

unsigned char *encrypt_payload(Encryption *encryption, const unsigned char *payload,
                               size_t payload_size, size_t *out_size)
{
        size_t msg_size = 0;
        unsigned char *out = calloc(ENCRYPT_DECRYPT_BUFFER_SIZE, sizeof(unsigned char));
        if (out == NULL) {
                printf("[ERROR] - encrypt_payload - Could not allocate memory for buffer\n");
                exit(1);
        }

        int success = encrypt_decrypt_msg(encryption, EncryptAction_ENCRYPT, payload, payload_size,
                                          out + DWORD, &msg_size);
        if (0 == success) {
                printf("[ERROR] - encrypt_payload - Could not encrypt payload\n");
                exit(1);
        }

        *out_size = DWORD + msg_size;

        // Save msg size in big endian
        out[0] = (msg_size >> 24) & 0xFF;
        out[1] = (msg_size >> 16) & 0xFF;
        out[2] = (msg_size >> 8) & 0xFF;
        out[3] = msg_size & 0xFF;

        return out;
}

unsigned char *decrypt_payload(Encryption *encryption, const unsigned char *payload,
                               size_t payload_size, size_t *out_size)
{
        unsigned char *out = calloc(ENCRYPT_DECRYPT_BUFFER_SIZE, sizeof(unsigned char));
        if (out == NULL) {
                printf("[ERROR] - decrypt_payload - Could not allocate memory for buffer\n");
                exit(1);
        }

        int success = encrypt_decrypt_msg(encryption, EncryptAction_DECRYPT, payload, payload_size,
                                          out, out_size);
        if (0 == success) {
                printf("[ERROR] - decrypt_payload - Could not decrypt payload\n");
                exit(1);
        }

        return out;
}
#endif /* ENCRYPT */
