#include "WindowsUpdater.h"

#include <ui-config.h>

WindowsUpdater::WindowsUpdater(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &WindowsUpdater::onVersionCheckFinished);
}

void WindowsUpdater::checkForUpdates() const
{
    manager->get(QNetworkRequest(QUrl("https://storage.hale-tech.net/manifest.json")));
}

void WindowsUpdater::onVersionCheckFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        const QByteArray response = reply->readAll();
        const QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QJsonObject jsonObj = jsonDoc.object();

        const QString latestVersion = jsonObj["version"].toString();
        const QString releaseDate = jsonObj["release_date"].toString();
        const QString downloadUrl = jsonObj["url"].toString();
        const QString changelog = jsonObj["changelog"].toString();

        // Compare versions (You can use QVersionNumber in <QVersionNumber> for safer parsing)
        if (latestVersion > VURA_VERSION_CANONICAL) {
            emit updateAvailable(latestVersion, releaseDate, downloadUrl, changelog);
        } else {
            emit updateNotAvailable();
        }
    } else {
        qWarning() << "Failed to check for updates: " << reply->errorString();
    }
    reply->deleteLater();
}
