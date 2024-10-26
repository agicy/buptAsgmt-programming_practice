#pragma once

#ifndef _HMAC_H_
#define _HMAC_H_

#ifdef __cplusplus
extern "C" {
#include <cstddef>
#else
#include <stddef.h>
#endif

/**
 * Computes the HMAC (Hash-based Message Authentication Code) of a given string using a specified key and algorithm.
 * This function utilizes the OpenSSL library to perform the HMAC computation. It is important to ensure that
 * the OpenSSL library is properly initialized and that the specified algorithm is supported by the library.
 *
 * The function takes the following parameters:
 *
 * @param str The input string to compute the HMAC for. This is the data that will be authenticated.
 * @param str_length The length of the input string. It is important to provide the correct length to avoid buffer overflows.
 * @param key The key used for the HMAC computation. This should be a secret key known only to the sender and receiver.
 * @param key_length The length of the key. Like the input string length, this is crucial for preventing buffer overflows.
 * @param algorithm The name of the hash algorithm to use (e.g., "SHA256"). This should correspond to a valid and available
 *                  hash algorithm in the OpenSSL library. You can check the available hash algorithms using the OpenSSL
 *                  command-line tool or by using EVP_MD_fetch in the EVP library to enumerate them.
 * @param buffer The buffer to store the resulting HMAC string in hexadecimal format. The buffer must be large enough to
 *               hold the HMAC output, which is typically the size of the hash algorithm's output (e.g., 32 bytes for SHA256).
 * @param buffer_size The size of the buffer. It must be at least as large as the HMAC output size to avoid buffer overflows.
 *
 * @return Pointer to the buffer containing the HMAC string on success, or NULL on failure. If the function fails, it may be
 *         due to reasons such as an unsupported algorithm, incorrect buffer size, or memory allocation issues.
 */
extern char *hmac(
    const char *const str, const size_t str_length,
    const char *const key, const size_t key_length,
    const char *const algorithm,
    char *const buffer, const size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif
