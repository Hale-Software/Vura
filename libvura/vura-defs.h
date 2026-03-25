/*******************************************************************************
     Copyright (c) 2026.  by Andrew Hale <halea2196@gmail.com>
 
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

#pragma once

#define VURA_ALIGN_CENTER (0)
#define VURA_ALIGN_LEFT (1 << 0)
#define VURA_ALIGN_RIGHT (1 << 1)
#define VURA_ALIGN_TOP (1 << 2)
#define VURA_ALIGN_BOTTOM (1 << 3)

#define VURA_OUTPUT_SUCCESS 0
#define VURA_OUTPUT_BAD_PATH -1
#define VURA_OUTPUT_CONNECT_FAILED -2
#define VURA_OUTPUT_INVALID_STREAM -3
#define VURA_OUTPUT_ERROR -4
#define VURA_OUTPUT_DISCONNECTED -5
#define VURA_OUTPUT_UNSUPPORTED -6
#define VURA_OUTPUT_NO_SPACE -7
#define VURA_OUTPUT_ENCODE_ERROR -8
#define VURA_OUTPUT_HDR_DISABLED -9

#define VURA_VIDEO_SUCCESS 0
#define VURA_VIDEO_FAIL -1
#define VURA_VIDEO_NOT_SUPPORTED -2
#define VURA_VIDEO_INVALID_PARAM -3
#define VURA_VIDEO_CURRENTLY_ACTIVE -4
