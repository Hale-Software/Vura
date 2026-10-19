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

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QVersionNumber>
#include <QFile>
#include <QDir>

#include <ui-config.h>

/**
 * UpdateChecker
 *
 * Owned by MainWindow. Call check() once at startup (or on a menu action).
 *
 * Flow:
 *   1. GET manifest URL → parse JSON → compare versions
 *   2. If newer: GET updater_url → save VuraUpdater.exe to %TEMP%
 *   3. Launch VuraUpdater.exe with args, then quit the main app
 *
 * VuraUpdater.exe is called with these arguments:
 *   --install-dir  <path>   Directory where Vura.exe lives
 *   --download-url <url>    URL of the new Vura installer/zip
 *   --app-exe      <name>   Executable name to relaunch after update
 *
 * Signals:
 *   noUpdateAvailable()         — version is current, nothing to do
 *   updateAvailable(QString)    — newer version string, for showing a dialog
 *   updateReadyToInstall()      — updater downloaded, about to quit
 *   error(QString)              — something went wrong
 */
class UpdateChecker : public QObject
{
    Q_OBJECT

public:
    static constexpr auto k_manifestUrl = "https://vura.hale-software.com/manifest.json";

    static constexpr auto k_currentVersion = VURA_VERSION_CANONICAL;

    explicit UpdateChecker(QObject* parent = nullptr);

    /**
     * Begin the update check. Non-blocking — results arrive via signals.
     * Safe to call multiple times; ignores calls while already in progress.
     */
    void check();

signals:
    void noUpdateAvailable();
    void updateAvailable(const QString& newVersion);
    void downloadProgress(qint64 received, qint64 total);
    void updateReadyToInstall();
    void error(const QString& message);

private slots:
    void onManifestReply(QNetworkReply* reply);
    void onUpdaterDownloadProgress(qint64 received, qint64 total);
    void onUpdaterDownloadFinished();

private:
    void downloadUpdater(const QString& updaterUrl);
    void launchUpdaterAndQuit();
    static bool isNewerVersion(const QString& remote);

    QNetworkAccessManager* m_nam            = nullptr;
    QNetworkReply*         m_downloadReply  = nullptr;
    QFile*                 m_tempFile       = nullptr;
    QString                m_updaterTempPath;
    QString                m_downloadUrl;
    bool                   m_busy           = false;

};
