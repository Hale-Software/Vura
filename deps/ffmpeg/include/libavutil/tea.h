/*******************************************************************************
     Copyright (c) 2015-2026 by Andrew Hale <halea2196@gmail.com>

     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#ifndef AVUTIL_TEA_H
#define AVUTIL_TEA_H

#include <stdint.h>

/**
 * @file
 * @brief Public header for libavutil TEA algorithm
 * @defgroup lavu_tea TEA
 * @ingroup lavu_crypto
 * @{
 */

extern const int av_tea_size;

struct AVTEA;

/**
  * Allocate an AVTEA context
  * To free the struct: av_free(ptr)
  */
struct AVTEA *av_tea_alloc(void);

/**
 * Initialize an AVTEA context.
 *
 * @param ctx an AVTEA context
 * @param key a key of 16 bytes used for encryption/decryption
 * @param rounds the number of rounds in TEA (64 is the "standard")
 */
void av_tea_init(struct AVTEA *ctx, const uint8_t key[16], int rounds);

/**
 * Encrypt or decrypt a buffer using a previously initialized context.
 *
 * @param ctx an AVTEA context
 * @param dst destination array, can be equal to src
 * @param src source array, can be equal to dst
 * @param count number of 8 byte blocks
 * @param iv initialization vector for CBC mode, if NULL then ECB will be used
 * @param decrypt 0 for encryption, 1 for decryption
 */
void av_tea_crypt(struct AVTEA *ctx, uint8_t *dst, const uint8_t *src,
                  int count, uint8_t *iv, int decrypt);

/**
 * @}
 */

#endif /* AVUTIL_TEA_H */
