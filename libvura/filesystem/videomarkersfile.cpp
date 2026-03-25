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

#include "videomarkersfile.h"


VideoMarkersFile::VideoMarkersFile() {}

void VideoMarkersFile::writeToFile(const char* fileName, const VideoMarker& marker)
{
    ofstream file(fileName, ios::binary);
    file.write((char*)&marker, sizeof(VideoMarker));
    file.close();
}

void VideoMarkersFile::appendToFile(const char* fileName, const VideoMarker& marker)
{
    ofstream file(fileName, ios::binary | ios::app);
    file.write((char*)&marker, sizeof(VideoMarker));
    file.close();
}

VideoMarker VideoMarkersFile::readFromFile(const char* fileName)
{
    VideoMarker marker;

    ifstream file(fileName, ios::binary);
    file.read((char*)&marker, sizeof(VideoMarker));
    file.close();

    return marker;
}

bool VideoMarkersFile::searchInFile(const char* fileName, const char* videoFile)
{
    VideoMarker marker;

    ifstream file(fileName, ios::binary);
    bool found = false;

    while (file.read((char*)&marker, sizeof(VideoMarker))) {
        if (strcmp(marker.file, videoFile) == 0) {
            found = true;
            break;
        }
    }

    file.close();
    return found;
}

void VideoMarkersFile::modifyRecordInFile(const char* fileName, const VideoMarker& marker) {}

void VideoMarkersFile::deleteRecordInFile(const char* fileName, const VideoMarker& marker) {}
