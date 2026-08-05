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

#include "crash-handler.h"
#include <libvura/config.h>

wchar_t CrashHandler::s_crashLogPath[MAX_PATH] = { 0 };

void CrashHandler::install()
{
    const QString crashDir = (QString(VURA_BUILD_TYPE) == "Debug") ? "debug/crashes" : QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/crashes";

    // Ensure the crash directory exists safely before a crash happens
    if (!QDir().mkpath(crashDir)) {
        qCritical() << "Failed to ensure crash directory exists!";
        return;
    }

    crashDir.toWCharArray(s_crashLogPath);

    // Null-terminate the string properly
    s_crashLogPath[crashDir.length()] = L'\0';

    // Hook the Windows exception filter
    SetUnhandledExceptionFilter(CrashFilter);
}

// Open the crash dump file
HANDLE CrashHandler::OpenCrashDumpFile(const wchar_t* dumpPath)
{
    return CreateFileW(
        dumpPath,
        GENERIC_WRITE,
        FILE_SHARE_READ,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );
}

// The actual callback that writes the dump
LONG WINAPI CrashHandler::CrashFilter(EXCEPTION_POINTERS *exceptionInfo)
{
    // Generate a unique filename using system time
    SYSTEMTIME st;
    GetLocalTime(&st);
    wchar_t dumpPath[MAX_PATH];
    wsprintfW(dumpPath, L"%s\\VuraCrash_%04d%02d%02d_%02d%02d%02d.dmp", s_crashLogPath, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

    // Open the file natively
    const HANDLE hFile = OpenCrashDumpFile(dumpPath);

    if (hFile != INVALID_HANDLE_VALUE) {
        // Configure the Minidump parameters
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ExceptionPointers = exceptionInfo;
        dumpInfo.ClientPointers = FALSE;

        // Write the dump to disk
        MiniDumpWriteDump(
            GetCurrentProcess(),
            GetCurrentProcessId(),
            hFile,
            MiniDumpNormal,
            &dumpInfo,
            NULL,
            NULL
            );

        CloseHandle(hFile);
    }

    // Tell Windows to execute the standard crash termination
    return EXCEPTION_EXECUTE_HANDLER;
}
