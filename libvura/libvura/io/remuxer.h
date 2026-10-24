#pragma once

#include <QObject>
#include <QString>
#include <atomic>


class Remuxer : public QObject
{
    Q_OBJECT
public:
    explicit Remuxer(QObject *parent = nullptr) : QObject(parent) {}

    bool remux(const QString &inputPath, const QString &outputPath);
    void cancel() { m_cancel = true; }

    QString lastError() const { return m_lastError; }

signals:
    void progress(int percent);
    void finished(bool ok, const QString &error);

private:
    bool fail(const QString &msg, int avError = 0);
    QString avErrorString(int err);

    std::atomic_bool m_cancel{false};
    QString m_lastError;

};
