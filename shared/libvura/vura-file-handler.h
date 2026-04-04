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

# pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

inline const char* VIDEO_MARKER_FORMAT_OUT = "(%s, %s, %d, %s)\n";

typedef struct VideoMarker {
    char filename[256];
    char markername;
    int timestamp;
    char type;

} VideoMarker;


class Serializer {

public:
    explicit Serializer();

    void serializeMarker(const char* outfile, VideoMarker marker);

};
