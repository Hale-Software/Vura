/*******************************************************************************
     Copyright (c) 2026 by Andrew Hale <halea2196@gmail.com>

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

/*
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef AVUTIL_REPLAYGAIN_H
#define AVUTIL_REPLAYGAIN_H

#include <stdint.h>

/**
 * ReplayGain information (see
 * http://wiki.hydrogenaudio.org/index.php?title=ReplayGain_1.0_specification).
 * The size of this struct is a part of the public ABI.
 */
typedef struct AVReplayGain {
    /**
     * Track replay gain in microbels (divide by 100000 to get the value in dB).
     * Should be set to INT32_MIN when unknown.
     */
    int32_t track_gain;
    /**
     * Peak track amplitude, with 100000 representing full scale (but values
     * may overflow). 0 when unknown.
     */
    uint32_t track_peak;
    /**
     * Same as track_gain, but for the whole album.
     */
    int32_t album_gain;
    /**
     * Same as track_peak, but for the whole album,
     */
    uint32_t album_peak;
} AVReplayGain;

#endif /* AVUTIL_REPLAYGAIN_H */
