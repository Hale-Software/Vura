#include "log-uploader.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QVariant>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>

namespace {

constexpr const char *kOmissionMarker =
    "\n\n=== [log truncated: middle section omitted by uploader] ===\n\n";

QString shortenBody(const QByteArray &body, int maxChars = 200)
{
    QString s = QString::fromUtf8(body).simplified();
    if (s.size() > maxChars)
        s = s.left(maxChars) + QStringLiteral("...");
    return s;
}

} // namespace

LogUploader::LogUploader(QObject *parent)
    : LogUploader(Config{}, parent)
{
}

LogUploader::LogUploader(Config config, QObject *parent)
    : QObject(parent)
    , m_config(std::move(config))
    , m_redactor(&LogUploader::defaultRedactor)
{
    m_nam.setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
}

LogUploader::~LogUploader()
{
    if (m_reply) {
        m_reply->disconnect(this);
        m_reply->abort();
    }
}

void LogUploader::setConfig(Config config)
{
    m_config = std::move(config);
}

void LogUploader::setRedactor(std::function<void(QString &)> redactor)
{
    m_redactor = std::move(redactor);
}

void LogUploader::defaultRedactor(QString &text)
{
    static const QRegularExpression keyValue(
        R"((stream[ _-]?key|api[ _-]?key|access[ _-]?token|secret|password|passwd|bearer)(\s*[:=]\s*|\s+)([^\s"',;]{4,}))",
        QRegularExpression::CaseInsensitiveOption);
    text.replace(keyValue, QStringLiteral("\\1\\2<redacted>"));

    // rtmp://host/app/<key>
    static const QRegularExpression rtmpKey(
        R"((rtmps?://[^\s/]+/[^\s/]+/)([^\s"']+))",
        QRegularExpression::CaseInsensitiveOption);
    text.replace(rtmpKey, QStringLiteral("\\1<redacted>"));

    // Query-string credentials.
    static const QRegularExpression queryKey(
        R"(([?&](?:key|token|auth|sig|signature)=)([^\s&"']+))",
        QRegularExpression::CaseInsensitiveOption);
    text.replace(queryKey, QStringLiteral("\\1<redacted>"));

    // C:\Users\Drew\... -> C:\Users\<user>\...
    static const QRegularExpression winUser(
        R"(([A-Za-z]:\\Users\\)([^\\\r\n"']+))",
        QRegularExpression::CaseInsensitiveOption);
    text.replace(winUser, QStringLiteral("\\1<user>"));

    // /home/drew/... and /Users/drew/...
    static const QRegularExpression posixUser(
        R"((/(?:home|Users)/)([^/\s"']+))");
    text.replace(posixUser, QStringLiteral("\\1<user>"));
}

void LogUploader::uploadFile(const QString &filePath, const QString &kind)
{
    if (isBusy()) {
        emit uploadFailed(Error::Busy, tr("An upload is already in progress."));
        return;
    }

    Error error = Error::None;
    QString message;
    const QByteArray raw = readLogFile(filePath, &error, &message);
    if (error != Error::None) {
        emit uploadFailed(error, message);
        return;
    }

    const QByteArray payload = prepare(QString::fromUtf8(raw));
    if (payload.isEmpty()) {
        emit uploadFailed(Error::FileEmpty, tr("The log file is empty."));
        return;
    }

    post(payload, kind, QFileInfo(filePath).fileName());
}

void LogUploader::uploadText(const QString &text, const QString &kind)
{
    if (isBusy()) {
        emit uploadFailed(Error::Busy, tr("An upload is already in progress."));
        return;
    }

    const QByteArray payload = prepare(text);
    if (payload.isEmpty()) {
        emit uploadFailed(Error::FileEmpty, tr("There is nothing to upload."));
        return;
    }

    post(payload, kind, QStringLiteral("session.log"));
}

void LogUploader::cancel()
{
    if (!m_reply)
        return;
    m_cancelled = true;
    m_reply->abort(); // finished() fires, handleReply() reports Cancelled
}

QByteArray LogUploader::readLogFile(const QString &filePath, Error *error, QString *message) const
{
    *error = Error::None;

    QFileInfo info(filePath);
    if (!info.exists() || !info.isFile()) {
        *error = Error::FileNotFound;
        *message = tr("Log file not found: %1").arg(QDir::toNativeSeparators(filePath));
        return {};
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        *error = Error::FileUnreadable;
        *message = tr("Could not open %1: %2")
                       .arg(QDir::toNativeSeparators(filePath), file.errorString());
        return {};
    }

    const qint64 size = file.size();
    if (size == 0) {
        *error = Error::FileEmpty;
        *message = tr("The log file is empty.");
        return {};
    }

    const qint64 budget = m_config.maxUploadBytes > 0 ? m_config.maxUploadBytes
                                                      : size;

    // Never pull a runaway log fully into memory: sample generously from both
    // ends and let truncateIfNeeded() do the final trim.
    if (size > budget * 2) {
        const QByteArray head = file.read(budget);
        file.seek(size - budget);
        const QByteArray tail = file.readAll();
        return head + QByteArray(kOmissionMarker) + tail;
    }

    return file.readAll();
}

QByteArray LogUploader::prepare(const QString &text) const
{
    QString working = text;
    if (m_redactor)
        m_redactor(working);
    return truncateIfNeeded(working.toUtf8());
}

QByteArray LogUploader::truncateIfNeeded(const QByteArray &utf8) const
{
    const qint64 budget = m_config.maxUploadBytes;
    if (budget <= 0 || utf8.size() <= budget)
        return utf8;

    const QByteArray marker(kOmissionMarker);

    // Keep a quarter of the budget from the top (build info, GPU, settings)
    // and the rest from the bottom (the actual failure).
    qsizetype headBytes = static_cast<qsizetype>(budget / 4);
    qsizetype tailBytes = static_cast<qsizetype>(budget) - headBytes - marker.size();
    if (tailBytes < 0) {
        headBytes = static_cast<qsizetype>(budget) / 2;
        tailBytes = static_cast<qsizetype>(budget) - headBytes;
    }

    // Cut on line boundaries so no UTF-8 sequence is split in half.
    qsizetype headEnd = utf8.lastIndexOf('\n', headBytes);
    if (headEnd < 0)
        headEnd = headBytes - 1;

    qsizetype tailStart = utf8.size() - tailBytes;
    const qsizetype nl = utf8.indexOf('\n', tailStart);
    if (nl >= 0 && nl + 1 < utf8.size())
        tailStart = nl + 1;

    return utf8.left(headEnd + 1) + marker + utf8.mid(tailStart);
}

void LogUploader::post(const QByteArray &payload, const QString &kind, const QString &fileName)
{
    if (!m_config.endpoint.isValid() || m_config.endpoint.scheme().isEmpty()) {
        emit uploadFailed(Error::Network, tr("No upload endpoint is configured."));
        return;
    }

    m_cancelled = false;

    QNetworkRequest request(m_config.endpoint);
    request.setHeader(QNetworkRequest::UserAgentHeader, m_config.userAgent);
    request.setTransferTimeout(m_config.timeoutMs);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);
    if (!m_config.authHeaderName.isEmpty())
        request.setRawHeader(m_config.authHeaderName, m_config.authHeaderValue);

    QNetworkReply *reply = nullptr;

    if (m_config.format == Format::Json) {
        QJsonObject body;
        body.insert(QString::fromUtf8(m_config.jsonDataField),
                    QString::fromUtf8(payload));
        if (!m_config.jsonKindField.isEmpty())
            body.insert(QString::fromUtf8(m_config.jsonKindField), kind);

        const QByteArray json = QJsonDocument(body).toJson(QJsonDocument::Compact);
        request.setHeader(QNetworkRequest::ContentTypeHeader,
                          QByteArrayLiteral("application/json"));
        reply = m_nam.post(request, json);
    } else {
        auto *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

        QHttpPart filePart;
        filePart.setHeader(QNetworkRequest::ContentTypeHeader,
                           QByteArrayLiteral("text/plain; charset=utf-8"));
        filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                           QVariant(QStringLiteral("form-data; name=\"%1\"; filename=\"%2\"")
                                        .arg(QString::fromUtf8(m_config.multipartFieldName),
                                             fileName)));
        filePart.setBody(payload);
        multiPart->append(filePart);

        if (!m_config.jsonKindField.isEmpty()) {
            QHttpPart kindPart;
            kindPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                               QVariant(QStringLiteral("form-data; name=\"%1\"")
                                            .arg(QString::fromUtf8(m_config.jsonKindField))));
            kindPart.setBody(kind.toUtf8());
            multiPart->append(kindPart);
        }

        reply = m_nam.post(request, multiPart);
        multiPart->setParent(reply); // freed with the reply
    }

    m_reply = reply;

    connect(reply, &QNetworkReply::uploadProgress, this, &LogUploader::uploadProgress);
    connect(reply, &QNetworkReply::finished, this, &LogUploader::handleReply);

    emit uploadStarted(payload.size());
}

void LogUploader::handleReply()
{
    QNetworkReply *reply = m_reply;
    if (!reply)
        return;

    m_reply.clear();
    reply->deleteLater();

    const QByteArray body = reply->readAll();
    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (m_cancelled) {
        m_cancelled = false;
        emit uploadFailed(Error::Cancelled, tr("Upload cancelled."));
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        if (status >= 400) {
            emit uploadFailed(Error::HttpStatus,
                              tr("Server returned HTTP %1. %2")
                                  .arg(status)
                                  .arg(shortenBody(body)));
        } else {
            emit uploadFailed(Error::Network,
                              tr("Upload failed: %1").arg(reply->errorString()));
        }
        return;
    }

    const QUrl logUrl = extractUrl(body);
    if (!logUrl.isValid() || logUrl.scheme().isEmpty()) {
        emit uploadFailed(Error::MalformedResponse,
                          tr("The server did not return a log URL. Response: %1")
                              .arg(shortenBody(body)));
        return;
    }

    QUrl viewerUrl;
    if (!m_config.viewerUrlTemplate.isEmpty()) {
        const QString encoded =
            QString::fromUtf8(QUrl::toPercentEncoding(logUrl.toString()));
        viewerUrl = QUrl(m_config.viewerUrlTemplate.arg(encoded));
    }

    emit uploadFinished(logUrl, viewerUrl);
}

QUrl LogUploader::extractUrl(const QByteArray &body) const
{
    QJsonParseError parseError{};
    const QJsonDocument doc = QJsonDocument::fromJson(body, &parseError);

    if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
        QJsonValue value = doc.object();
        const QList<QByteArray> path = m_config.responseUrlField.split('.');
        for (const QByteArray &segment : path) {
            if (!value.isObject())
                return {};
            value = value.toObject().value(QString::fromUtf8(segment));
        }
        if (value.isString()) {
            const QUrl url(value.toString());
            // Endpoints that answer with a relative path ("/logs/abc123").
            return url.isRelative() ? m_config.endpoint.resolved(url) : url;
        }
        return {};
    }

    // Plain-text endpoints just echo the URL.
    const QString trimmed = QString::fromUtf8(body).trimmed();
    if (trimmed.startsWith(QLatin1String("http")))
        return QUrl(trimmed);

    return {};
}
