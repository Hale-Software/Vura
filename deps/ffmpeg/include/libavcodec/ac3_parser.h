/*******************************************************************************
     Copyright (c) 2003-2026 by Andrew Hale <halea2196@gmail.com>

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

#ifndef AVCODEC_AC3_PARSER_H
#define AVCODEC_AC3_PARSER_H

#include <stddef.h>
#include <stdint.h>

/**
 * Extract the bitstream ID and the frame size from AC-3 data.
 */
int av_ac3_parse_header(const uint8_t *buf, size_t size,
                        uint8_t *bitstream_id, uint16_t *frame_size);


#endif /* AVCODEC_AC3_PARSER_H */
