#include "update.h"


UpdateManager::UpdateManager(QObject *parent)
{
    connect(&m_networkManager, &QNetworkAccessManager::finished, this, &UpdateManager::onResult);
}

void UpdateManager::checkForUpdates()
{

}

void UpdateManager::onResult(QNetworkReply *reply)
{

}
