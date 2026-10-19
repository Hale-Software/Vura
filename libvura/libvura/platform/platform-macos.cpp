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

#include "platform.h"
#include "../logging/categories.h"

#include <QString>
#include <QByteArray>
#include <QPair>
#include <QThread>

#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <mach/processor_info.h>
#include <mach/vm_statistics.h>


static QString getNativeCpuName()
{
    // Query the sysctl kernel state
    char buffer[256];
    size_t bufferSize = sizeof(buffer);
    if (sysctlbyname("machdep.cpu.brand_string", &buffer, &bufferSize, nullptr, 0) == 0) {
        return QString::fromUtf8(buffer).trimmed();
    }
    return QStringLiteral("Unknown macOS CPU");
}

static int getNativeCpuSpeedMhz()
{
    // Queries the base CPU frequency in Hz
    int64_t hz = 0;
    size_t size = sizeof(hz);
    if (sysctlbyname("hw.cpufrequency", &hz, &size, nullptr, 0) == 0) {
        return static_cast<int>(hz / 1000000);
    }
    return -1;
}

static QPair<int, int> getNativeCoreCounts()
{
    int physicalCores = 0;
    int logicalCores = 0;

    // Query sysctl directly for hardware core configurations
    size_t size = sizeof(physicalCores);
    if (sysctlbyname("hw.physicalcpu", &physicalCores, &size, nullptr, 0) != 0) {
        physicalCores = 1;
    }

    size = sizeof(logicalCores);
    if (sysctlbyname("hw.logicalcpu", &logicalCores, &size, nullptr, 0) != 0) {
        logicalCores = physicalCores;
    }

    return qMakePair(physicalCores, logicalCores);
}

static QPair<qulonglong, qulonglong> getNativeRamSizes()
{
    qulonglong totalRam = 0;
    qulonglong freeRam = 0;

    // 1. Get Total Physical RAM
    int mib[2] = { CTL_HW, HW_MEMSIZE };
    size_t length = sizeof(totalRam);
    sysctl(mib, 2, &totalRam, &length, nullptr, 0);

    // 2. Get Free RAM via Mach Virtual Memory statistics
    mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
    vm_statistics_data_t vmStats;
    if (host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vmStats, &count) == KERN_SUCCESS) {
        qulonglong pageSize = static_cast<qulonglong>(sysconf(_SC_PAGESIZE));
        freeRam = static_cast<qulonglong>(vmStats.free_count) * pageSize;
    }

    return qMakePair(totalRam, freeRam);
}

CpuTicks getCpuSample()
{
    CpuTicks sample;

    mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
    host_cpu_load_info_data_t cpuLoad;
    if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&cpuLoad, &count) == KERN_SUCCESS) {
        sample.idle = cpuLoad.cpu_ticks[CPU_STATE_IDLE];
        sample.total = cpuLoad.cpu_ticks[CPU_STATE_USER] +
                       cpuLoad.cpu_ticks[CPU_STATE_SYSTEM] +
                       cpuLoad.cpu_ticks[CPU_STATE_NICE] +
                       cpuLoad.cpu_ticks[CPU_STATE_IDLE];
    }

    return sample;
}

double getNativeCpuLoadPercentage()
{
    CpuTicks start = getCpuSample();
    QThread::msleep(100);
    CpuTicks end = getCpuSample();

    const qulonglong idleDelta = end.idle - start.idle;
    const qulonglong totalDelta = end.total - start.total;

    if (totalDelta == 0)
        return 0.0;

    double usage = (1.0 - (static_cast<double>(idleDelta) / totalDelta)) * 100.0;
    if (usage < 0.0)
        usage = 0.0;
    if (usage > 100.0)
        usage = 100.0;

    return usage;
}

static bool isRunningAsAdmin()
{
    return (geteuid() == 0);
}

void logDeviceInfo() {}
