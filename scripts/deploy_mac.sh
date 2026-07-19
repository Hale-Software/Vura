#!/bin/bash
#
#     Copyright (c) 2026 by Andrew Hale <halea2196@gmail.com>
#
#     This program is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version.
#
#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
#
#     You should have received a copy of the GNU General Public License
#     along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#

QT_BIN="/Users/username/Qt/6.x.x/macos/bin"
BUILD_APP="./MediaPlayer.app"

# 1. macdeployqt embeds Qt frameworks and rewrites internal link paths (rpaths)
$QT_BIN/macdeployqt $BUILD_APP -verbose=1

# 2. Fix FFmpeg linking inside the bundle manually if using homebrew libs
# Macdeployqt doesn't always automatically catch 3rd party non-framework dylibs.
# You may need to use 'install_name_tool' if FFmpeg isn't statically linked.
