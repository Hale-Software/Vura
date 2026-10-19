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

#include <windows.h>
#include <shlobj.h>
#include <comdef.h>
#include <Wbemidl.h>


bool isRunningAsAdmin()
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

bool isMicrosoftDefenderActive()
{
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        qCritical() << "Failed to initialize COM library.";
        return false;
    }

    hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
    if (FAILED(hr) && hr != RPC_E_TOO_LATE) {
        CoUninitialize();
        return false;
    }

    IWbemLocator *pLoc = NULL;
    hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&pLoc);
    if (FAILED(hr)) {
        CoUninitialize();
        return false;
    }

    IWbemServices *pSvc = NULL;
    // SecurityCenter2 is the correct namespace for desktop antivirus status
    hr = pLoc->ConnectServer(_bstr_t(L"ROOT\\SecurityCenter2"), NULL, NULL, 0, NULL, 0, 0, &pSvc);
    if (FAILED(hr)) {
        pLoc->Release();
        CoUninitialize();
        return false;
    }

    hr = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
        RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
    if (FAILED(hr)) {
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return false;
    }

    IEnumWbemClassObject* pEnumerator = NULL;
    bstr_t query("SELECT * FROM AntiVirusProduct");
    hr = pSvc->ExecQuery(_bstr_t("WQL"), query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);

    if (FAILED(hr)) {
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return false;
    }

    IWbemClassObject *pclsObj = NULL;
    ULONG uReturn = 0;
    bool isDefenderActive = false;

    while (pEnumerator) {
        hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
        if (0 == uReturn) break;

        VARIANT vtProp;
        // Check the product name
        hr = pclsObj->Get(L"displayName", 0, &vtProp, 0, 0);
        QString displayName = QString::fromWCharArray(vtProp.bstrVal);
        VariantClear(&vtProp);

        if (displayName.contains("Windows Defender", Qt::CaseInsensitive) ||
            displayName.contains("Microsoft Defender", Qt::CaseInsensitive)) {

            // productState contains bits for real-time protection and enabled status
            hr = pclsObj->Get(L"productState", 0, &vtProp, 0, 0);
            int productState = vtProp.intVal;
            VariantClear(&vtProp);

            // Bitmask breakdown for Center2:
            // 0x1000 = Antivirus is active/running
            // 0x0010 = Real-time protection is enabled
            bool isActive = (productState & 0x1000);
            bool isRealTimeOn = (productState & 0x0010);

            if (isActive && isRealTimeOn) {
                isDefenderActive = true;
                pclsObj->Release();
                break;
            }
        }
        pclsObj->Release();
    }

    pEnumerator->Release();
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();

    return isDefenderActive;
}

void logDeviceInfo()
{
    // Log Operating System and Kernel
    QString osStr = QString("OS: %1 (%2)")
                        .arg(QSysInfo::prettyProductName())
                        .arg(QSysInfo::kernelType() + " " + QSysInfo::kernelVersion());

    // Log CPU Architecture
    QString cpuStr = QString("CPU Architecture: %1 (Build: %2)")
    .arg(QSysInfo::currentCpuArchitecture())
    .arg(QSysInfo::buildCpuArchitecture());

    // Thread Count (Good indicator of logical cores available for FFmpeg)
    QString threadStr = QString("Logical Cores: %1")
    .arg(QThread::idealThreadCount());

    // Primary Display Information
    QString screenStr = "Display: Unknown";
    if (QScreen *primaryScreen = QGuiApplication::primaryScreen()) {
        QRect geometry = primaryScreen->geometry();
        screenStr = QString("Display: %1x%2 @ %3Hz")
        .arg(geometry.width())
        .arg(geometry.height())
        .arg(primaryScreen->refreshRate());
    }

    // Disk Space (App Data Drive)
    QStorageInfo storage(QDir::currentPath());
    QString storageStr = QString("Storage: %1 GB Free / %2 GB Total")
    .arg(storage.bytesAvailable() / (1024 * 1024 * 1024))
    .arg(storage.bytesTotal() / (1024 * 1024 * 1024));

    // --- Write specs to log ---
    QString finalSpecLog = QString("\n--- DEVICE SPECIFICATIONS ---\n%1\n%2\n%3\n%4\n%5\n-----------------------------")
    .arg(osStr, cpuStr, threadStr, screenStr, storageStr);

    qDebug() << finalSpecLog;
}
