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

#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

struct VideoMarker {
    char file[256];
    char type[64];
    float marker;
};


class VideoMarkersFile {

public:
    VideoMarkersFile();

    void writeToFile(const char* fileName, const VideoMarker& marker);
    void appendToFile(const char* fileName, const VideoMarker& marker);
    VideoMarker readFromFile(const char* fileName);

    bool searchInFile(const char* fileName, const char* videoFile);

    void modifyRecordInFile(const char* fileName, const VideoMarker& marker);
    void deleteRecordInFile(const char* fileName, const VideoMarker& marker);


};

