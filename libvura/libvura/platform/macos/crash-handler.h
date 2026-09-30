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

#pragma once

#include <QString>
#include <QDir>
#include <QDateTime>
#include <QStandardPaths>

#include <csignal>
#include <exception>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <execinfo.h>
#include <unistd.h>


class CrashHandler
{
public:
    static void install();

private:
    static char s_crashLogPath[1024];
    static void handleSignal(int signal);
    static void handleTerminate();
    static void writeCrashReport(const char* reason);

};
