#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QDebug>

class WindowsUpdater : public QObject
{
    Q_OBJECT

public:
    explicit WindowsUpdater(QObject *parent = nullptr);
    void checkForUpdates() const;

signals:
    void updateAvailable(QString versionString, QString releaseDateString, QString downloadUrl, QString changelog);
    void updateNotAvailable();

private slots:
    void onVersionCheckFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager = nullptr;

};
