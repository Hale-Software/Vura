#include "platform.h"
#include "../logging/categories.h"

#include <QString>
#include <QByteArray>
#include <QFile>
#include <QTextStream>
#include <QPair>
#include <QSet>
#include <QThread>

#include <unistd.h>
#include <sys/sysinfo.h>


static QString getNativeCpuName()
{
    // Parse /proc/cpuinfo line-by-line
    QFile file(QStringLiteral("/proc/cpuinfo"));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            // Check for modern x86/ARM keys
            if (line.startsWith(QStringLiteral("model name"), Qt::CaseInsensitive) ||
                line.startsWith(QStringLiteral("Processor"), Qt::CaseInsensitive)) {
                int colonIdx = line.indexOf(QLatin1Char(':'));
                if (colonIdx != -1) {
                    file.close();
                    return line.mid(colonIdx + 1).trimmed();
                }
                }
        }
        file.close();
    }
    return QStringLiteral("Unknown Linux CPU");
}

static int getNativeCpuSpeedMhz()
{
    // 1. Check for scaling max frequency (most accurate for maximum capability)
    QFile maxFreqFile(QStringLiteral("/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq"));
    if (maxFreqFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&maxFreqFile);
        long long khz = in.readLine().toLongLong();
        maxFreqFile.close();
        if (khz > 0) return static_cast<int>(khz / 1000);
    }

    // 2. Fallback: Parse /proc/cpuinfo for current frequency (usually 'cpu MHz')
    QFile file(QStringLiteral("/proc/cpuinfo"));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith(QStringLiteral("cpu MHz"), Qt::CaseInsensitive)) {
                int colonIdx = line.indexOf(QLatin1Char(':'));
                if (colonIdx != -1) {
                    file.close();
                    return static_cast<int>(line.mid(colonIdx + 1).trimmed().toDouble());
                }
            }
        }
        file.close();
    }
    return -1;
}

static QPair<int, int> getNativeCoreCounts()
{
    int physicalCores = 0;
    int logicalCores = 0;

    // 1. Get Logical Cores via unistd sysconf
    logicalCores = static_cast<int>(sysconf(_SC_NPROCESSORS_ONLN));

    // 2. Get Physical Cores by counting unique core IDs in /proc/cpuinfo
    QFile file(QStringLiteral("/proc/cpuinfo"));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QSet<QString> uniqueCores;
        QString currentSocket = QStringLiteral("0");
        QString currentCoreId = QStringLiteral("-1");

        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.startsWith(QStringLiteral("physical id"), Qt::CaseInsensitive)) {
                currentSocket = line.section(QLatin1Char(':'), 1).trimmed();
            } else if (line.startsWith(QStringLiteral("core id"), Qt::CaseInsensitive)) {
                currentCoreId = line.section(QLatin1Char(':'), 1).trimmed();
                // Combine physical socket ID and local core ID to accurately track multi-CPU setups
                uniqueCores.insert(currentSocket + QLatin1Char('_') + currentCoreId);
            }
        }
        file.close();

        if (!uniqueCores.isEmpty()) {
            physicalCores = uniqueCores.size();
        }
    }

    if (physicalCores == 0) physicalCores = logicalCores;

    return qMakePair(physicalCores, logicalCores);
}

static QPair<qulonglong, qulonglong> getNativeRamSizes()
{
    qulonglong totalRam = 0;
    qulonglong freeRam = 0;

    struct sysinfo si;
    if (sysinfo(&si) == 0) {
        // Multiply by mem_unit to support systems with more than 4GB where scaling applies
        totalRam = static_cast<qulonglong>(si.totalram) * si.mem_unit;
        // freeram is strictly empty memory, safer to add buffer/cached if needed,
        // but for "free", freeram + freeram_shared is standard.
        freeRam = static_cast<qulonglong>(si.freeram) * si.mem_unit;
    }

    return qMakePair(totalRam, freeRam);
}

CpuTicks getCpuSample()
{
    CpuTicks sample;

    QFile file(QStringLiteral("/proc/stat"));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString line = in.readLine(); // First line is always the aggregate 'cpu' line
        if (line.startsWith(QStringLiteral("cpu"))) {
            QStringList tokens = line.split(QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
            if (tokens.size() > 4) {
                qulonglong user   = tokens[1].toULongLong();
                qulonglong nice   = tokens[2].toULongLong();
                qulonglong system = tokens[3].toULongLong();
                qulonglong idle   = tokens[4].toULongLong();

                sample.idle = idle;
                sample.total = user + nice + system + idle;
                // Add optional tracking fields if present (iowait, irq, etc)
                for (int i = 5; i < tokens.size(); ++i) {
                    sample.total += tokens[i].toULongLong();
                }
            }
        }
        file.close();
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

void logDeviceInfo() {}
