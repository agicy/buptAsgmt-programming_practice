#include "hmac.h"
#include <openssl/ssl.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

char *hmac(
    const char *const str, const size_t str_length,
    const char *const key, const size_t key_length,
    const char *const algorithm,
    char *const buffer, const size_t buffer_size) {
    const EVP_MD *engine = EVP_get_digestbyname(algorithm);
    if (!engine) {
        snprintf(buffer, buffer_size,
                 "invalid algorithm %s for hmac", algorithm);
        return NULL;
    }

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
    // OpenSSL 3.0 and above
    EVP_MAC *mac = EVP_MAC_fetch(NULL, "HMAC", NULL);
    if (!mac) {
        snprintf(buffer, buffer_size,
                 "EVP_MAC_fetch fail");
        return NULL;
    }

    EVP_MAC_CTX *ctx = EVP_MAC_CTX_new(mac);
    if (!ctx) {
        EVP_MAC_free(mac);
        snprintf(buffer, buffer_size,
                 "EVP_MAC_CTX_new fail");
        return NULL;
    }

    OSSL_PARAM params[3];
    params[0] = OSSL_PARAM_construct_utf8_string(
        "digest", (char *)algorithm, 0);
    params[1] = OSSL_PARAM_construct_octet_string(
        "key", (unsigned char *)key, key_length);
    params[2] = OSSL_PARAM_construct_end();

    if (!EVP_MAC_init(ctx, NULL, 0, params)) {
        EVP_MAC_CTX_free(ctx);
        EVP_MAC_free(mac);
        snprintf(buffer, buffer_size,
                 "EVP_MAC_init fail");
        return NULL;
    }

    if (!EVP_MAC_update(ctx, (unsigned char *)str, str_length)) {
        snprintf(buffer, buffer_size,
                 "EVP_MAC_update fail");
        EVP_MAC_CTX_free(ctx);
        EVP_MAC_free(mac);
        return NULL;
    }

    unsigned char output[EVP_MAX_MD_SIZE];
    size_t output_length = sizeof(output);
    if (!EVP_MAC_final(ctx, output, &output_length, sizeof(output))) {
        snprintf(buffer, buffer_size,
                 "EVP_MAC_final fail");
        EVP_MAC_CTX_free(ctx);
        EVP_MAC_free(mac);
        return NULL;
    }

    EVP_MAC_CTX_free(ctx);
    EVP_MAC_free(mac);
#else
    // OpenSSL < 3.0
    HMAC_CTX *ctx = HMAC_CTX_new();
    if (HMAC_Init_ex(ctx, key, key_length, engine, NULL) <= 0) {
        snprintf(buffer, buffer_size,
                 "HMAC_Init_ex fail");
        HMAC_CTX_free(ctx);
        return NULL;
    }

    if (HMAC_Update(ctx, (unsigned char *)str, str_length) <= 0) {
        snprintf(buffer, buffer_size,
                 "HMAC_Update fail");
        HMAC_CTX_free(ctx);
        return NULL;
    }

    unsigned char output[EVP_MAX_MD_SIZE];
    unsigned int output_length;
    if (HMAC_Final(ctx, output, &output_length) <= 0) {
        snprintf(buffer, buffer_size,
                 "HMAC_Final fail");
        HMAC_CTX_free(ctx);
        return NULL;
    }

    if (output_length > EVP_MAX_MD_SIZE) {
        snprintf(buffer, buffer_size,
                 "HMAC_Final return invalid output length %u\n", output_length);
        HMAC_CTX_free(ctx);
        return NULL;
    }

    HMAC_CTX_free(ctx);
#endif

    if (buffer_size <= output_length * 2 + 1) {
        snprintf(buffer, buffer_size,
                 "hmac buffer too small(%zu), but %zu needed",
                 buffer_size, (size_t)output_length * 2 + 1);
        return NULL;
    }

    static const char *const hex_digits = "0123456789abcdef";

    char *p = buffer;
    for (size_t i = 0; i < output_length; i++) {
        *p++ = hex_digits[output[i] >> 4];
        *p++ = hex_digits[output[i] & 0x0f];
    }
    *p = '\0';

    return buffer;
}
