/*******************************************************************************
     Copyright (c) 2009-2026 by Andrew Hale <halea2196@gmail.com>

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

#ifndef AVUTIL_RANDOM_SEED_H
#define AVUTIL_RANDOM_SEED_H

#include <stddef.h>
#include <stdint.h>
/**
 * @addtogroup lavu_crypto
 * @{
 */

/**
 * Get a seed to use in conjunction with random functions.
 * This function tries to provide a good seed at a best effort bases.
 * Its possible to call this function multiple times if more bits are needed.
 * It can be quite slow, which is why it should only be used as seed for a faster
 * PRNG. The quality of the seed depends on the platform.
 */
uint32_t av_get_random_seed(void);

/**
 * Generate cryptographically secure random data, i.e. suitable for use as
 * encryption keys and similar.
 *
 * @param buf buffer into which the random data will be written
 * @param len size of buf in bytes
 *
 * @retval 0                         success, len bytes of random data was written
 *                                   into buf
 * @retval "a negative AVERROR code" random data could not be generated
 */
int av_random_bytes(uint8_t *buf, size_t len);

/**
 * @}
 */

#endif /* AVUTIL_RANDOM_SEED_H */
