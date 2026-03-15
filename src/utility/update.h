#ifndef HPLAYER_UPDATE_H
#define HPLAYER_UPDATE_H

#include <QObject>
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>


class UpdateManager : public QObject
{
    Q_OBJECT

public:
    explicit UpdateManager(QObject *parent = nullptr);

public slots:
    void checkForUpdates();

signals:
    bool updateAvailable();

private slots:
    void onResult(QNetworkReply *reply);

private:
    QNetworkAccessManager m_networkManager;

};


#endif //HPLAYER_UPDATE_H