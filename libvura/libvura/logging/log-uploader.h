#pragma once

#include <QByteArray>
#include <QNetworkAccessManager>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QUrl>

#include <functional>


QT_BEGIN_NAMESPACE
class QNetworkReply;
QT_END_NAMESPACE

/*!
 * Uploads a log file to a paste-style HTTP endpoint and hands back the public
 * URL, the way OBS Studio's "Upload Current Log File" does.
 *
 * Flow:
 *   read file -> redact secrets -> truncate to budget -> POST -> parse URL
 *
 * All work is asynchronous; the object is single-flight (one upload at a time)
 * and safe to keep alive for the lifetime of a dialog or the main window.
 */
class LogUploader : public QObject
{
    Q_OBJECT

public:
    enum class Error {
        None,
        Busy,               //!< an upload is already in flight
        FileNotFound,
        FileUnreadable,
        FileEmpty,
        Network,            //!< transport failure, DNS, TLS, timeout, ...
        HttpStatus,         //!< server answered with >= 400
        MalformedResponse,  //!< 200, but no usable URL in the body
        Cancelled
    };
    Q_ENUM(Error)

    enum class Format {
        Json,       //!< {"data": "<log>", "type": "log"}  -- what OBS does
        Multipart   //!< multipart/form-data with a file part
    };
    Q_ENUM(Format)

    struct Config {
        QUrl endpoint;

        Format format = Format::Json;

        // JSON mode field names.
        QByteArray jsonDataField = "data";
        QByteArray jsonKindField = "type";

        // Multipart mode part name / filename.
        QByteArray multipartFieldName = "file";

        // Where to find the resulting URL in the JSON response. Supports a
        // dotted path, e.g. "result.url". If the body is not JSON, a bare URL
        // in the body is accepted as a fallback.
        QByteArray responseUrlField = "url";

        // Optional viewer/analyzer link built from the returned URL.
        // Use %1 as the placeholder, e.g. "https://example.com/analyze?log=%1".
        QString viewerUrlTemplate;

        // Payload budget after redaction. Larger logs keep the head (system
        // info) and the tail (whatever just went wrong).
        qint64 maxUploadBytes = 5 * 1024 * 1024;

        // Per-transfer inactivity timeout.
        int timeoutMs = 30000;

        QByteArray userAgent = "Vura/1.0 (log-uploader)";

        // Optional static auth header, e.g. ("Authorization", "Bearer ...").
        QByteArray authHeaderName;
        QByteArray authHeaderValue;
    };

    explicit LogUploader(QObject *parent = nullptr);
    explicit LogUploader(Config config, QObject *parent = nullptr);
    ~LogUploader() override;

    void setConfig(Config config);
    const Config &config() const { return m_config; }

    //! Replaces the redaction pass. Pass {} to disable redaction entirely.
    void setRedactor(std::function<void(QString &)> redactor);

    //! Masks stream keys, tokens, bearer headers and Windows user paths.
    static void defaultRedactor(QString &text);

    bool isBusy() const { return !m_reply.isNull(); }

public slots:
    void uploadFile(const QString &filePath, const QString &kind = QStringLiteral("log"));
    void uploadText(const QString &text, const QString &kind = QStringLiteral("log"));
    void cancel();

signals:
    void uploadStarted(qint64 payloadBytes);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void uploadFinished(const QUrl &logUrl, const QUrl &viewerUrl);
    void uploadFailed(LogUploader::Error error, const QString &message);

private:
    QByteArray readLogFile(const QString &filePath, Error *error, QString *message) const;
    QByteArray prepare(const QString &text) const;
    QByteArray truncateIfNeeded(const QByteArray &utf8) const;
    void post(const QByteArray &payload, const QString &kind, const QString &fileName);
    void handleReply();
    QUrl extractUrl(const QByteArray &body) const;

    Config m_config;
    QNetworkAccessManager m_nam;
    QPointer<QNetworkReply> m_reply;
    std::function<void(QString &)> m_redactor;
    bool m_cancelled = false;
};
