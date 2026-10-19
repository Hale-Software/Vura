#pragma once

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "../../models/file-entry.h"


class UpdateManager : public QObject
{
    Q_OBJECT
public:
    explicit UpdateManager(QObject *parent = nullptr);
    ~UpdateManager() override;

signals:
    void errorOccurred(QString errorMessage);
    void updateAvailable(bool available);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished(bool success, const QString &message);

public slots:
    void checkForUpdates();
    void downloadUpdate();

private slots:
    void updateCheckReplyFinished(QNetworkReply *reply);

private:
    bool isPlatformUpdateAvailable(QByteArray &replyData, QString *error);
    bool verifyFileIntegrity(const QString &filePath, const QString &expectedHash);

    QNetworkAccessManager *m_networkManager;
    QVector<FileEntry> m_files;
    QString m_rootUrl;
    bool m_isBeta = false;

};
