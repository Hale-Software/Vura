/*******************************************************************************
     Copyright (c) 2000-2026 by Andrew Hale <halea2196@gmail.com>

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

#ifndef AVUTIL_TIME_H
#define AVUTIL_TIME_H

#include <stdint.h>

/**
 * Get the current time in microseconds.
 */
int64_t av_gettime(void);

/**
 * Get the current time in microseconds since some unspecified starting point.
 * On platforms that support it, the time comes from a monotonic clock
 * This property makes this time source ideal for measuring relative time.
 * The returned values may not be monotonic on platforms where a monotonic
 * clock is not available.
 */
int64_t av_gettime_relative(void);

/**
 * Indicates with a boolean result if the av_gettime_relative() time source
 * is monotonic.
 */
int av_gettime_relative_is_monotonic(void);

/**
 * Sleep for a period of time.  Although the duration is expressed in
 * microseconds, the actual delay may be rounded to the precision of the
 * system timer.
 *
 * @param  usec Number of microseconds to sleep.
 * @return zero on success or (negative) error code.
 */
int av_usleep(unsigned usec);

#endif /* AVUTIL_TIME_H */
