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
#include <libvura/config.h>

#include <QString>
#include <QByteArray>
#include <QPair>
#include <QThread>
#include <QDateTime>

#include <windows.h>
#include <wscapi.h>
#include <iwscapi.h>
#include <shlobj.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <intrin.h>
#include <netfw.h>
#include <vector>


enum class DefenderState
{
    Active,
    DisabledOrPassive,
    Error
};

enum class FirewallState
{
    Enabled,
    Disabled,
    Error
};

static QString getNativeCpuName()
{
    // Method 1: Hardware CPUID Instruction (Works on Intel/AMD)
    int cpuInfo[4] = { 0 };
    __cpuid(cpuInfo, 0x80000000);
    unsigned int nExIds = cpuInfo[0];

    if (nExIds >= 0x80000004) {
        char cpuBrandString[0x40] = { 0 };
        __cpuid((int*)(cpuBrandString),       0x80000002);
        __cpuid((int*)(cpuBrandString + 16),  0x80000003);
        __cpuid((int*)(cpuBrandString + 32),  0x80000004);
        return QString::fromUtf8(cpuBrandString).trimmed();
    }

    // Method 2: Fallback to Windows Registry
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, R"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        char buffer[1024];
        DWORD bufferSize = sizeof(buffer);
        if (RegQueryValueExA(hKey, "ProcessorNameString", nullptr, nullptr, (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return QString::fromUtf8(buffer).trimmed();
        }
        RegCloseKey(hKey);
    }
    return QStringLiteral("Unknown Windows CPU");
}

static int getNativeCpuSpeedMhz()
{
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, R"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD mhz = 0;
        DWORD bufferSize = sizeof(mhz);
        if (RegQueryValueExA(hKey, "~MHz", nullptr, nullptr, (LPBYTE)&mhz, &bufferSize) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return static_cast<int>(mhz);
        }
        RegCloseKey(hKey);
    }
    return -1;
}

static QPair<int, int> getNativeCoreCounts()
{
    int physicalCores = 0;
    int logicalCores = 0;

    // 1. Get Logical Cores via basic system info
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    logicalCores = static_cast<int>(sysInfo.dwNumberOfProcessors);

    // 2. Get Physical Cores via Logical Processor Information API
    DWORD bufferSize = 0;
    GetLogicalProcessorInformation(nullptr, &bufferSize);

    if (bufferSize > 0) {
        std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
        if (GetLogicalProcessorInformation(buffer.data(), &bufferSize)) {
            for (const auto& info : buffer) {
                if (info.Relationship == RelationProcessorCore) {
                    physicalCores++;
                }
            }
        }
    }

    // Fallback safety if API fail
    if (physicalCores == 0) physicalCores = logicalCores;

    return qMakePair(physicalCores, logicalCores);
}

static QPair<qulonglong, qulonglong> getNativeRamSizes()
{
    qulonglong totalRam = 0;
    qulonglong freeRam = 0;

    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memInfo)) {
        totalRam = memInfo.ullTotalPhys;
        freeRam = memInfo.ullAvailPhys; // Returns immediately available physical memory
    }

    return qMakePair(totalRam, freeRam);
}

CpuTicks getCpuSample()
{
    CpuTicks sample;

    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        // Convert FILETIME to 64-bit unsigned integers
        ULARGE_INTEGER liIdle, liKernel, liUser;
        liIdle.LowPart   = idleTime.dwLowDateTime;   liIdle.HighPart   = idleTime.dwHighDateTime;
        liKernel.LowPart = kernelTime.dwLowDateTime; liKernel.HighPart = kernelTime.dwHighDateTime;
        liUser.LowPart   = userTime.dwLowDateTime;   liUser.HighPart   = userTime.dwHighDateTime;

        sample.idle = liIdle.QuadPart;
        // On Windows, kernelTime includes idleTime, so total is kernelTime + userTime
        sample.total = liKernel.QuadPart + liUser.QuadPart;
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
    BOOL isAdmin = FALSE;
    PSID administratorsGroup = NULL;

    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0,
        &administratorsGroup)) {
        CheckTokenMembership(NULL, administratorsGroup, &isAdmin);
        FreeSid(administratorsGroup);
    }
    return isAdmin == TRUE;
}

static DefenderState getMicrosoftDefenderState()
{
    // Initialize COM library for the current thread
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        return DefenderState::Error;
    }

    IWSCProductList* pProductList = nullptr;
    hr = CoCreateInstance(__uuidof(WSCProductList), nullptr, CLSCTX_INPROC_SERVER,
                          __uuidof(IWSCProductList), reinterpret_cast<LPVOID*>(&pProductList));

    if (FAILED(hr)) {
        CoUninitialize();
        return DefenderState::Error;
    }

    // Initialize list to query Antivirus Providers
    hr = pProductList->Initialize(WSC_SECURITY_PROVIDER_ANTIVIRUS);
    if (FAILED(hr)) {
        pProductList->Release();
        CoUninitialize();
        return DefenderState::Error;
    }

    LONG productCount = 0;
    pProductList->get_Count(&productCount);

    bool isDefenderActive = false;

    for (LONG i = 0; i < productCount; ++i) {
        IWscProduct* pProduct = nullptr;
        if (SUCCEEDED(pProductList->get_Item(i, &pProduct))) {
            BSTR bstrName = nullptr;
            WSC_SECURITY_PRODUCT_STATE productState;

            if (SUCCEEDED(pProduct->get_ProductName(&bstrName)) &&
                SUCCEEDED(pProduct->get_ProductState(&productState))) {

                QString prodName = QString::fromWCharArray(bstrName);
                SysFreeString(bstrName);

                // Look explicitly for Windows / Microsoft Defender
                if (prodName.contains(QStringLiteral("Defender"), Qt::CaseInsensitive)) {
                    // WSC_SECURITY_PRODUCT_STATE_ON means the AV engine is active and running
                    if (productState == WSC_SECURITY_PRODUCT_STATE_ON) {
                        isDefenderActive = true;
                    }
                }
            }
            pProduct->Release();
        }
    }

    pProductList->Release();
    CoUninitialize();

    return isDefenderActive ? DefenderState::Active : DefenderState::DisabledOrPassive;
}

static FirewallState getWindowsFirewallState()
{
    // 1. Initialize COM library for the current thread
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        return FirewallState::Error;
    }

    INetFwMgr* pFwMgr = nullptr;
    INetFwPolicy* pFwPolicy = nullptr;
    INetFwProfile* pFwProfile = nullptr;
    FirewallState state = FirewallState::Error;

    // 2. Instantiate the Firewall Manager
    hr = CoCreateInstance(__uuidof(NetFwMgr), nullptr, CLSCTX_INPROC_SERVER,
                          __uuidof(INetFwMgr), reinterpret_cast<void**>(&pFwMgr));

    if (SUCCEEDED(hr)) {
        // 3. Get the local firewall policy
        hr = pFwMgr->get_LocalPolicy(&pFwPolicy);
        if (SUCCEEDED(hr)) {
            // 4. Get the currently active profile
            hr = pFwPolicy->get_CurrentProfile(&pFwProfile);
            if (SUCCEEDED(hr)) {
                VARIANT_BOOL fwEnabled = VARIANT_FALSE;
                // 5. Query if the firewall is turned on
                hr = pFwProfile->get_FirewallEnabled(&fwEnabled);
                if (SUCCEEDED(hr)) {
                    state = (fwEnabled == VARIANT_TRUE) ? FirewallState::Enabled
                                                        : FirewallState::Disabled;
                }
            }
        }
    }

    // Clean up resources
    if (pFwProfile) pFwProfile->Release();
    if (pFwPolicy) pFwPolicy->Release();
    if (pFwMgr) pFwMgr->Release();
    CoUninitialize();

    return state;
}

void logDeviceInfo()
{
    // Log CPU Name
    QString cpuNameStr = QString("CPU Name: %1")
                                .arg(getNativeCpuName());

    // Log CPU Speed
    QString cpuSpeedStr = QString("CPU Speed: %1MHz")
                                 .arg(getNativeCpuSpeedMhz());

    // Log CPU Core Counts
    QPair<int, int> cpuCoreCount = getNativeCoreCounts();
    QString cpuCoreStr = QString("Physical Cores: %1, Logical Cores: %2")
                                .arg(cpuCoreCount.first)
                                .arg(cpuCoreCount.second);

    // Log RAM Sizes (MB)
    QPair<qulonglong, qulonglong> ramSizes = getNativeRamSizes();
    QString ramStr = QString("Physical Memory: %1MB Total, %2MB Free")
                            .arg(ramSizes.first / (1024.0 * 1024.0))
                            .arg(ramSizes.second / (1024.0 * 1024.0));

    // Log Operating System and Kernel
    QString osStr = QString("OS: %1 (%2)")
                        .arg(QSysInfo::prettyProductName())
                        .arg(QSysInfo::kernelType() + " " + QSysInfo::kernelVersion());

    // Log Running As Administrator
    QString adminStr = "Running As Administrator: false";
    if (isRunningAsAdmin())
        adminStr = "Running As Administrator: true";

    // Log Windows 10/11 Gaming Feature Game DVR

    // Log Windows 10/11 Gaming Feature Game Mode

    // Log Microsoft Defender Antivirus Status
    QString microsoftDefenderStatusStr = "unknown";
    DefenderState microsoftDefenderState = getMicrosoftDefenderState();
    switch (microsoftDefenderState) {
        case DefenderState::Active:
            microsoftDefenderStatusStr = "enabled";
            break;
        case DefenderState::DisabledOrPassive:
            microsoftDefenderStatusStr = "disabled/passive";
            break;
        case DefenderState::Error:
            microsoftDefenderStatusStr = "error";
            break;
        default:
            break;
    }
    QString microsoftDefenderStr = QString("Microsoft Defender Antivirus: %1")
                                          .arg(microsoftDefenderStatusStr);

    // Log Windows Firewall Status
    QString windowsFirewallStatusStr = "unknown";
    FirewallState windowsFirewallState = getWindowsFirewallState();
    switch (windowsFirewallState) {
        case FirewallState::Enabled:
            windowsFirewallStatusStr = "enabled";
            break;
        case FirewallState::Disabled:
            windowsFirewallStatusStr = "disabled";
            break;
        case FirewallState::Error:
            windowsFirewallStatusStr = "error";
            break;
        default:
            break;
    }
    QString windowsFirewallStr = QString("Windows Firewall: %1")
                                        .arg(windowsFirewallStatusStr);

    // Log Primary Display Information
    QString primDisplayWidthStr = "unknown";
    QString primDisplayHeightStr = "unknown";
    QString primDisplayRefRateStr = "unknown";
    if (QScreen *primaryScreen = QGuiApplication::primaryScreen()) {
        QRect geometry = primaryScreen->geometry();
        if (geometry.isValid()) {
            primDisplayWidthStr = QString::number(geometry.width());
            primDisplayHeightStr = QString::number(geometry.height());
        }
        primDisplayRefRateStr = QString("%1Hz").arg(primaryScreen->refreshRate());
    }

    // Log App Data Drive Disk Space
    QStorageInfo storage(QDir::currentPath());
    QString storageStr = QString("Storage: %1 GB Free / %2 GB Total")
                                .arg(storage.bytesAvailable() / (1024 * 1024 * 1024))
                                .arg(storage.bytesTotal() / (1024 * 1024 * 1024));

    // Log Current Date/Time
    QDateTime now = QDateTime::currentDateTime();
    QString curDateTimeStr = QString("Current Date/Time: %1, %2")
                                    .arg(now.toString("yyyy-MM-dd"))
                                    .arg(now.toString("hh:mm:ss"));

    // Log Application Info
    QString applicationStr = QString("%1 %2 %3 (%4) (%5, %6)")
                                    .arg(VURA_PRODUCT_NAME)
                                    .arg(VURA_VERSION_CANONICAL)
                                    .arg(VURA_BUILD_STRING)
                                    .arg(VURA_BUILD_TYPE);

    // Write to log
    qCInfo(Core) << cpuNameStr;
    qCInfo(Core) << cpuSpeedStr;
    qCInfo(Core) << cpuCoreStr;
    qCInfo(Core) << ramStr;
    qCInfo(Core) << osStr;
    qCInfo(Core) << adminStr;
    qCInfo(Core) << "Windows 10/11 Gaming Features:";
    qCInfo(Core) << "-  Game DVR: ";
    qCInfo(Core) << "-  Game Mode: ";

    qCInfo(Core) << "Sec. Software Status:";
    qCInfo(Core) << "-  " << microsoftDefenderStr;
    qCInfo(Core) << "-  " << windowsFirewallStr;
    qCInfo(Core) << "-  " << storageStr;

    qCInfo(Core) << "Primary Display Information:";
    qCInfo(Core) << "-  Width: " << primDisplayWidthStr;
    qCInfo(Core) << "-  Height: " << primDisplayHeightStr;
    qCInfo(Core) << "-  Refresh Rate: " << primDisplayRefRateStr;

    qCInfo(Core) << curDateTimeStr;
    qCInfo(Core) << applicationStr;
    qCInfo(Core) << "---------------------------------";
    qCInfo(Core) << "---------------------------------";
}
