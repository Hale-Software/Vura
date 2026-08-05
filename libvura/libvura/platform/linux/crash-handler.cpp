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


char CrashHandler::s_crashLogPath[1024] = {0};

void CrashHandler::install()
{
    const QString crashDir = (QString(VURA_BUILD_TYPE) == "Debug") ? "debug/crashes" : QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/crashes";

    // Ensure the crash directory exists safely before a crash happens
    if (!QDir().mkpath(crashDir)) {
        qCritical() << "Failed to ensure crash directory exists!";
        return;
    }

    // Format a safe filepath string: e.g., "crashes/crash.log"
    const QString fileName = "VuraCrash_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".log";
    QString fullPath = QDir(crashDir).filePath(fileName);

    // Copy the path into our static C-array so its safely accessible when the heap dies
    std::strncpy(s_crashLogPath, fullPath.toUtf8().constData(), sizeof(s_crashLogPath) - 1);

    // Hook into OS-level crashes (Segfaults, Math errors, etc.)
    std::signal(SIGSEGV, handleSignal); // Segmentation violation (invalid memory)
    std::signal(SIGILL, handleSignal); // Illegal instruction
    std::signal(SIGFPE, handleSignal); // Floating-point exception
    std::signal(SIGABRT, handleSignal); // Abort signal

    // Hook into unhandled C++ exceptions
    std::set_terminate(handleTerminate);
}

void CrashHandler::handleSignal(int signal)
{
    const char *reason = "Unknown signal";
    switch (signal) {
        case SIGSEGV: reason = "SIGSEGV (Segmentation Fault)"; break;
        case SIGILL:  reason = "SIGILL (Illegal Instruction)"; break;
        case SIGFPE:  reason = "SIGFPE (Floating-Point Exception)"; break;
        case SIGABRT: reason = "SIGABRT (Abort)"; break;
    }

    writeCrashReport(reason);

    // Terminate the application immediately
    std::exit(signal);
}

void CrashHandler::handleTerminate()
{
    writeCrashReport("std::terminate called (Unhandled C++ Exception)");
    std::exit(EXIT_FAILURE);
}

void CrashHandler::writeCrashReport(const char *reason)
{
    FILE *file = std::fopen(s_crashLogPath, "a");
    if (!file) return;

    std::fputs("=================================\n", file);
    std::fputs("VURA VIDEO PLAYER - CRASH REPORT\n\n", file);
    std::fputs("Reason: ", file);
    std::fputs(reason, file);
    std::fputs("\n", file);

    // macOS and Linux can generate a basic stack trace natively
    void *callstack[128];
    int frames = backtrace(callstack, 128);
    char** strs = backtrace_symbols(callstack, frames);
    if (strs) {
        std::fputs("--- Stack Trace ---\n", file);
        for (int i = 0; i < frames; ++i) {
            std::fputs(strs[i], file);
            std::fputs("\n", file);
        }
        std::free(strs);
    }

    std::fputs("=================================\n\n", file);
    std::fclose(file);
}
