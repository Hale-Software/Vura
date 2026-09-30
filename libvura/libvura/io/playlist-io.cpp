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

#include "playlist-io.h"

#include <QBuffer>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QRegularExpression>
#include <QSaveFile>
#include <QStringConverter>
#include <QTime>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

namespace playlistio {
namespace {

/// Playlists are lists of strings. Anything this size is either not a playlist
/// or a denial-of-service waiting to happen in the XML reader.
constexpr qint64 kMaxPlaylistBytes = 32ll * 1024 * 1024;

// ---------------------------------------------------------------------------
// Text decoding
// ---------------------------------------------------------------------------

/// Windows-1252 is not one of QStringConverter's built-in encodings, but it is
/// what an undeclared .m3u or .asx written on Windows almost always contains.
/// Only 0x80-0x9F differ from Latin-1, so a 32-entry table covers it.
QString decodeCp1252(const QByteArray &in)
{
    static const char16_t high[32] = {
        0x20AC, 0x0081, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
        0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0x008D, 0x017D, 0x008F,
        0x0090, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
        0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0x009D, 0x017E, 0x0178,
    };

    QString out;
    out.reserve(in.size());
    for (char raw : in) {
        const auto c = static_cast<unsigned char>(raw);
        out.append(c >= 0x80 && c <= 0x9F ? QChar(high[c - 0x80]) : QChar(c));
    }
    return out;
}

/// Byte order mark first, then UTF-8, then Windows-1252. Refusing to guess and
/// using the machine's locale codec instead would make the same file parse
/// differently on two computers, which is exactly what we are trying to avoid.
QString decodeText(const QByteArray &bytes, bool utf8Only)
{
    if (const auto encoding = QStringConverter::encodingForData(bytes)) {
        QStringDecoder decoder(*encoding);
        QString text = decoder(bytes);
        if (!decoder.hasError())
            return text;
    }

    QStringDecoder utf8(QStringDecoder::Utf8, QStringDecoder::Flag::Stateless);
    QString text = utf8(bytes);
    if (!utf8.hasError() || utf8Only)
        return text;

    return decodeCp1252(bytes);
}

QString stripBom(QString text)
{
    if (!text.isEmpty() && text.at(0) == QChar(0xFEFF))
        text.remove(0, 1);
    return text;
}

// ---------------------------------------------------------------------------
// Location handling
//
// This is where the cross-platform pain lives. A playlist can contain, in any
// combination: POSIX paths, Windows paths with drive letters and backslashes,
// UNC paths, proper file:// URIs, http(s) URLs, Winamp's schemeless "file:C:\"
// strings, and relative paths in any of those flavours.
// ---------------------------------------------------------------------------

bool hasUrlScheme(const QString &s)
{
    // Two or more leading letters before the colon, so that "C:\Music" is read
    // as a drive letter rather than as a scheme named "c".
    static const QRegularExpression re(QStringLiteral(R"(^[A-Za-z][A-Za-z0-9+.\-]+:)"));
    return re.match(s).hasMatch();
}

bool looksLikeWindowsPath(const QString &s)
{
    return s.size() >= 2 && s.at(0).isLetter() && s.at(1) == u':';
}

QUrl uncToUrl(QString s)
{
    s.replace(u'\\', u'/');            // \\server\share\x -> //server/share/x
    const QString body = s.mid(2);
    const qsizetype slash = body.indexOf(u'/');

    QUrl url;
    url.setScheme(QStringLiteral("file"));
    url.setHost(slash < 0 ? body : body.left(slash));
    url.setPath(slash < 0 ? QStringLiteral("/") : body.mid(slash));
    return url;
}

QUrl resolveLocation(QString raw, const QUrl &base)
{
    raw = stripBom(raw.trimmed());
    if (raw.isEmpty())
        return {};

    // Winamp writes "file:C:\Music\x.mp3" into .b4s: a scheme with no authority
    // and no leading slash, which QUrl would keep as an opaque path forever.
    if (raw.startsWith(QLatin1String("file:"), Qt::CaseInsensitive)) {
        const QString rest = raw.mid(5);
        if (!rest.startsWith(u'/')) {
            raw = rest;
        } else if (rest.startsWith(QLatin1String("//")) && !rest.startsWith(QLatin1String("///"))
                   && looksLikeWindowsPath(rest.mid(2))) {
            raw = rest.mid(2);         // file://C:/x, where C: became the host
        }
    }

    if (raw.startsWith(QLatin1String("\\\\")))
        return uncToUrl(raw);

    if (looksLikeWindowsPath(raw)) {
        QString path = raw;
        path.replace(u'\\', u'/');
        // On Unix this yields file:///C:/Music/x.mp3, which will not exist.
        // relocateMissingFiles is what rescues those entries.
        return QUrl::fromLocalFile(QDir::cleanPath(path));
    }

    if (hasUrlScheme(raw)) {
        const QUrl url(raw, QUrl::TolerantMode);
        if (url.isValid() && !url.isRelative())
            return url;
    }

    // A plain path. Backslashes are treated as separators unconditionally: a
    // POSIX file name containing one is far rarer than a Windows-written list.
    QString path = raw;
    path.replace(u'\\', u'/');

    if (path.startsWith(u'/'))
        return QUrl::fromLocalFile(QDir::cleanPath(path));

    if (!base.isValid())
        return {};                     // nothing to resolve against

    if (base.isLocalFile()) {
        const QString dir = QFileInfo(base.toLocalFile()).absolutePath();
        return QUrl::fromLocalFile(QDir::cleanPath(dir + u'/' + path));
    }
    return base.resolved(QUrl(path, QUrl::TolerantMode));
}

/// Last-resort search for a local file that the playlist claims exists.
QUrl relocate(const QUrl &url, const QUrl &base, const ReadOptions &options)
{
    if (!url.isLocalFile())
        return url;

    const QString path = url.toLocalFile();
    if (QFileInfo::exists(path))
        return url;

    // Some writers percent-encode the path even though it is not a URI, so the
    // name on disk may be the decoded form.
    const QString decoded = QUrl::fromPercentEncoding(path.toUtf8());
    if (decoded != path && QFileInfo::exists(decoded))
        return QUrl::fromLocalFile(decoded);

    if (!options.relocateMissingFiles || !base.isLocalFile())
        return url;

    const QDir dir(QFileInfo(base.toLocalFile()).absolutePath());

    const qsizetype cut = path.lastIndexOf(u'/');
    const QString name = path.mid(cut + 1);
    if (name.isEmpty())
        return url;
    if (dir.exists(name))
        return QUrl::fromLocalFile(dir.absoluteFilePath(name));

    // Try keeping one parent component, for the common Album/track.mp3 layout.
    if (cut > 0) {
        const qsizetype cut2 = path.lastIndexOf(u'/', cut - 1);
        if (cut2 >= 0) {
            const QString tail = path.mid(cut2 + 1);
            if (dir.exists(tail))
                return QUrl::fromLocalFile(dir.absoluteFilePath(tail));
        }
    }
    return url;
}

bool acceptEntry(const Entry &entry, const ReadOptions &options)
{
    if (!entry.url.isValid() || entry.url.isEmpty())
        return false;
    if (options.keepMissingFiles || !entry.url.isLocalFile())
        return true;
    return QFileInfo::exists(entry.url.toLocalFile());
}

// ---------------------------------------------------------------------------
// Location handling, writing direction
// ---------------------------------------------------------------------------

QString percentEncodePath(const QString &path)
{
    return QString::fromLatin1(QUrl::toPercentEncoding(path, "/"));
}

/// `asUri` picks between "what a text playlist wants" (a bare path) and "what
/// an XML playlist wants" (a URI).
QString locationFor(const QUrl &url, const QUrl &base, const WriteOptions &options, bool asUri)
{
    if (!url.isLocalFile())
        return url.toString(QUrl::FullyEncoded);

    const QString absolute = QDir::fromNativeSeparators(url.toLocalFile());

    if (options.pathStyle != WriteOptions::Absolute && base.isLocalFile()) {
        const QDir dir(QFileInfo(base.toLocalFile()).absolutePath());
        const QString relative = dir.relativeFilePath(absolute);
        const bool climbs = relative.startsWith(QLatin1String(".."));
        if (!relative.isEmpty() && (options.pathStyle == WriteOptions::Relative || !climbs))
            return asUri ? percentEncodePath(relative) : relative;
    }

    return asUri ? url.toString(QUrl::FullyEncoded) : absolute;
}

// ---------------------------------------------------------------------------
// M3U / M3U8
// ---------------------------------------------------------------------------

Entry parseExtInf(QStringView spec)
{
    Entry entry;

    const qsizetype comma = spec.indexOf(u',');
    const QStringView head = comma < 0 ? spec : spec.left(comma);
    if (comma >= 0)
        entry.title = spec.mid(comma + 1).trimmed().toString();

    // head is "123", "-1", "123.5", or HLS-flavoured "123 tvg-id=\"x\"".
    qsizetype n = 0;
    while (n < head.size()) {
        const QChar c = head.at(n);
        if (!c.isDigit() && c != u'-' && c != u'+' && c != u'.')
            break;
        ++n;
    }

    bool ok = false;
    const double seconds = head.left(n).toDouble(&ok);
    if (ok && seconds > 0)
        entry.duration = media::Msec(qRound64(seconds * 1000.0));
    return entry;
}

QList<Entry> parseM3u(const QString &text, const QUrl &base, const ReadOptions &options)
{
    // An HLS manifest is a list of stream variants or media segments, not a
    // list of tracks. Expanding it here would queue up thousands of two-second
    // chunks; the right move is to hand the manifest URL to QMediaPlayer, whose
    // FFmpeg backend plays it directly.
    if (text.contains(QLatin1String("#EXT-X-STREAM-INF"), Qt::CaseInsensitive)
        || text.contains(QLatin1String("#EXT-X-TARGETDURATION"), Qt::CaseInsensitive)) {
        if (!base.isValid())
            return {};
        Entry stream;
        stream.url = base;
        stream.title = QFileInfo(base.path()).completeBaseName();
        return {stream};
    }

    QList<Entry> out;
    Entry pending;
    bool havePending = false;

    const QStringList lines = text.split(u'\n');
    for (const QString &rawLine : lines) {
        const QString line = rawLine.trimmed();   // also eats the \r of CRLF
        if (line.isEmpty())
            continue;

        if (line.startsWith(u'#')) {
            if (line.startsWith(QLatin1String("#EXTINF:"), Qt::CaseInsensitive)) {
                pending = parseExtInf(QStringView(line).mid(8));
                havePending = true;
            }
            // #EXTM3U, #PLAYLIST, #EXTVLCOPT, #EXTGRP and plain comments: skip.
            continue;
        }

        Entry entry = havePending ? pending : Entry{};
        havePending = false;
        entry.url = relocate(resolveLocation(line, base), base, options);
        if (acceptEntry(entry, options))
            out.append(entry);
    }
    return out;
}

QByteArray serializeM3u(const QList<Entry> &entries, const QUrl &base, const WriteOptions &options)
{
    // CRLF, because some elderly Windows players choke on bare LF, and every
    // other parser in existence trims the \r.
    static const QLatin1String eol("\r\n");

    QString text;
    if (options.writeExtendedInfo)
        text += QLatin1String("#EXTM3U") + eol;

    for (const Entry &entry : entries) {
        if (options.writeExtendedInfo) {
            const qint64 seconds = entry.duration > 0 ? (entry.duration + 500) / 1000 : -1;
            QString title = entry.title;
            title.replace(u'\n', u' ').replace(u'\r', u' ');
            text += QLatin1String("#EXTINF:") + QString::number(seconds) + u',' + title + eol;
        }
        text += locationFor(entry.url, base, options, /*asUri=*/false) + eol;
    }

    // Both flavours are written as UTF-8. There is no portable way to write a
    // "locale encoded" .m3u that another machine will read back correctly, and
    // every current player copes with UTF-8. Offer .m3u8 in the save dialog if
    // you want the file name to be honest about it.
    return text.toUtf8();
}

// ---------------------------------------------------------------------------
// Shared XML helpers
// ---------------------------------------------------------------------------

QString attributeValue(const QXmlStreamAttributes &attributes, QLatin1String name)
{
    for (const QXmlStreamAttribute &attribute : attributes) {
        if (attribute.name().compare(name, Qt::CaseInsensitive) == 0)
            return attribute.value().toString();
    }
    return {};
}

bool isElement(QStringView name, QLatin1String wanted)
{
    return name.compare(wanted, Qt::CaseInsensitive) == 0;
}

/// Reads the text of the element the reader is currently positioned on, and
/// leaves the reader on its end element either way.
QString elementText(QXmlStreamReader &reader)
{
    return reader.readElementText(QXmlStreamReader::IncludeChildElements);
}

/// Plenty of .b4s and .asx files declare UTF-8 and then contain Windows-1252
/// bytes. When the byte-oriented parse fails, re-run it over decoded text,
/// where the (now ignored) encoding declaration cannot get in the way.
template <typename ParseFn>
QList<Entry> parseXmlTolerant(const QByteArray &bytes, ParseFn parseFn)
{
    {
        QXmlStreamReader reader(bytes);
        QList<Entry> out = parseFn(reader);
        if (!reader.hasError() || !out.isEmpty())
            return out;
    }
    QXmlStreamReader reader(decodeText(bytes, /*utf8Only=*/false));
    return parseFn(reader);
}

// ---------------------------------------------------------------------------
// XSPF
// ---------------------------------------------------------------------------

QList<Entry> parseXspf(const QByteArray &bytes, const QUrl &base, const ReadOptions &options)
{
    return parseXmlTolerant(bytes, [&](QXmlStreamReader &reader) {
        QList<Entry> out;

        while (!reader.atEnd()) {
            if (reader.readNext() != QXmlStreamReader::StartElement)
                continue;
            if (!isElement(reader.name(), QLatin1String("track")))
                continue;

            Entry entry;
            QString location;
            QString creator;

            // readNextStartElement walks the direct children and stops at
            // </track>, so <extension> blobs cannot leak their contents in.
            while (reader.readNextStartElement()) {
                const QStringView name = reader.name();
                if (isElement(name, QLatin1String("location")) && location.isEmpty()) {
                    location = elementText(reader);
                } else if (isElement(name, QLatin1String("title"))) {
                    entry.title = elementText(reader);
                } else if (isElement(name, QLatin1String("creator"))) {
                    creator = elementText(reader);
                } else if (isElement(name, QLatin1String("duration"))) {
                    bool ok = false;
                    const qint64 ms = elementText(reader).trimmed().toLongLong(&ok);
                    if (ok && ms > 0)
                        entry.duration = media::Msec(ms);
                } else {
                    reader.skipCurrentElement();
                }
            }

            if (!creator.isEmpty() && !entry.title.isEmpty())
                entry.title = creator + QLatin1String(" - ") + entry.title;

            entry.url = relocate(resolveLocation(location, base), base, options);
            if (acceptEntry(entry, options))
                out.append(entry);
        }
        return out;
    });
}

QByteArray serializeXspf(const QList<Entry> &entries, const QUrl &base, const WriteOptions &options)
{
    QByteArray output;
    QXmlStreamWriter writer(&output);
    writer.setAutoFormatting(true);

    writer.writeStartDocument();
    writer.writeDefaultNamespace(QStringLiteral("http://xspf.org/ns/0/"));
    writer.writeStartElement(QStringLiteral("playlist"));
    writer.writeAttribute(QStringLiteral("version"), QStringLiteral("1"));
    writer.writeStartElement(QStringLiteral("trackList"));

    for (const Entry &entry : entries) {
        writer.writeStartElement(QStringLiteral("track"));
        writer.writeTextElement(QStringLiteral("location"),
                                locationFor(entry.url, base, options, /*asUri=*/true));
        if (!entry.title.isEmpty())
            writer.writeTextElement(QStringLiteral("title"), entry.title);
        if (entry.duration > 0)   // XSPF durations are milliseconds
            writer.writeTextElement(QStringLiteral("duration"), QString::number(entry.duration));
        writer.writeEndElement();
    }

    writer.writeEndElement();   // trackList
    writer.writeEndElement();   // playlist
    writer.writeEndDocument();
    return output;
}

// ---------------------------------------------------------------------------
// B4S (Winamp 3)
// ---------------------------------------------------------------------------

QList<Entry> parseB4s(const QByteArray &bytes, const QUrl &base, const ReadOptions &options)
{
    return parseXmlTolerant(bytes, [&](QXmlStreamReader &reader) {
        QList<Entry> out;

        while (!reader.atEnd()) {
            if (reader.readNext() != QXmlStreamReader::StartElement)
                continue;
            if (!isElement(reader.name(), QLatin1String("entry")))
                continue;

            Entry entry;
            const QString playstring =
                attributeValue(reader.attributes(), QLatin1String("Playstring"));

            while (reader.readNextStartElement()) {
                const QStringView name = reader.name();
                if (isElement(name, QLatin1String("Name"))) {
                    entry.title = elementText(reader);
                } else if (isElement(name, QLatin1String("Length"))) {
                    bool ok = false;
                    // Winamp writes milliseconds here, unlike #EXTINF.
                    const qint64 ms = elementText(reader).trimmed().toLongLong(&ok);
                    if (ok && ms > 0)
                        entry.duration = media::Msec(ms);
                } else {
                    reader.skipCurrentElement();
                }
            }

            entry.url = relocate(resolveLocation(playstring, base), base, options);
            if (acceptEntry(entry, options))
                out.append(entry);
        }
        return out;
    });
}

QByteArray serializeB4s(const QList<Entry> &entries, const QUrl &base, const WriteOptions &options)
{
    QByteArray output;
    QXmlStreamWriter writer(&output);
    writer.setAutoFormatting(true);

    writer.writeStartDocument();
    writer.writeStartElement(QStringLiteral("WinampXML"));
    writer.writeStartElement(QStringLiteral("playlist"));
    writer.writeAttribute(QStringLiteral("num_entries"), QString::number(entries.size()));
    writer.writeAttribute(QStringLiteral("label"), QStringLiteral("Playlist"));

    for (const Entry &entry : entries) {
        QString playstring = locationFor(entry.url, base, options, /*asUri=*/false);
        if (entry.url.isLocalFile())
            playstring.prepend(QLatin1String("file:"));   // Winamp's own spelling

        writer.writeStartElement(QStringLiteral("entry"));
        writer.writeAttribute(QStringLiteral("Playstring"), playstring);
        writer.writeTextElement(QStringLiteral("Name"),
                                entry.title.isEmpty() ? entry.url.fileName() : entry.title);
        if (entry.duration > 0)
            writer.writeTextElement(QStringLiteral("Length"), QString::number(entry.duration));
        writer.writeEndElement();
    }

    writer.writeEndElement();   // playlist
    writer.writeEndElement();   // WinampXML
    writer.writeEndDocument();
    return output;
}

// ---------------------------------------------------------------------------
// ASX
//
// ASX only pretends to be XML. Tag names are case-insensitive, attribute values
// are often unquoted, ampersands in query strings are usually unescaped, and
// closing tags are optional in practice. Feeding one to QXmlStreamReader fails
// on a large share of real files, so this is a deliberately sloppy scanner.
// ---------------------------------------------------------------------------

QString unescapeEntities(QString text)
{
    static const QRegularExpression numeric(QStringLiteral(R"(&#(x[0-9A-Fa-f]+|[0-9]+);)"));
    QRegularExpressionMatchIterator it = numeric.globalMatch(text);
    QString result;
    qsizetype last = 0;
    while (it.hasNext()) {
        const QRegularExpressionMatch match = it.next();
        const QString digits = match.captured(1);
        bool ok = false;
        const char32_t code = digits.startsWith(u'x') || digits.startsWith(u'X')
                                  ? char32_t(digits.mid(1).toUInt(&ok, 16))
                                  : char32_t(digits.toUInt(&ok, 10));
        result += QStringView(text).mid(last, match.capturedStart() - last);
        result += ok ? QString::fromUcs4(&code, 1) : match.captured(0);
        last = match.capturedEnd();
    }
    result += QStringView(text).mid(last);

    result.replace(QLatin1String("&lt;"), QLatin1String("<"));
    result.replace(QLatin1String("&gt;"), QLatin1String(">"));
    result.replace(QLatin1String("&quot;"), QLatin1String("\""));
    result.replace(QLatin1String("&apos;"), QLatin1String("'"));
    result.replace(QLatin1String("&amp;"), QLatin1String("&"));
    return result;
}

/// Pulls the first non-empty capture group out of a match, which lets one
/// regex cover double-quoted, single-quoted and bare attribute values.
QString firstCapture(const QRegularExpressionMatch &match, int from, int to)
{
    for (int i = from; i <= to; ++i) {
        const QString captured = match.captured(i);
        if (!captured.isEmpty())
            return captured;
    }
    return {};
}

const QRegularExpression &asxRefRegex()
{
    static const QRegularExpression re(
        QStringLiteral(R"RX(<\s*ref\b[^>]*?\bhref\s*=\s*(?:"([^"]*)"|'([^']*)'|([^\s>]+)))RX"),
        QRegularExpression::CaseInsensitiveOption
            | QRegularExpression::DotMatchesEverythingOption);
    return re;
}

media::Msec parseAsxDuration(const QString &value)
{
    if (value.isEmpty())
        return -1;
    double seconds = 0;
    const QStringList parts = value.split(u':');
    for (const QString &part : parts)
        seconds = seconds * 60 + part.toDouble();   // toDouble is C-locale, good
    return seconds > 0 ? media::Msec(qRound64(seconds * 1000.0)) : -1;
}

QList<Entry> parseAsx(const QString &text, const QUrl &base, const ReadOptions &options)
{
    static const QRegularExpression entryRe(
        QStringLiteral(R"(<\s*entry\b[^>]*>(.*?)<\s*/\s*entry\s*>)"),
        QRegularExpression::CaseInsensitiveOption
            | QRegularExpression::DotMatchesEverythingOption);
    static const QRegularExpression titleRe(
        QStringLiteral(R"(<\s*title\s*>(.*?)<\s*/\s*title\s*>)"),
        QRegularExpression::CaseInsensitiveOption
            | QRegularExpression::DotMatchesEverythingOption);
    static const QRegularExpression durationRe(
        QStringLiteral(R"RX(<\s*duration\b[^>]*?\bvalue\s*=\s*(?:"([^"]*)"|'([^']*)'|([^\s>/]+)))RX"),
        QRegularExpression::CaseInsensitiveOption
            | QRegularExpression::DotMatchesEverythingOption);

    QList<Entry> out;
    bool sawEntry = false;

    QRegularExpressionMatchIterator entries = entryRe.globalMatch(text);
    while (entries.hasNext()) {
        sawEntry = true;
        const QString body = entries.next().captured(1);

        const QRegularExpressionMatch refMatch = asxRefRegex().match(body);
        if (!refMatch.hasMatch())
            continue;   // e.g. an <ENTRYREF> to another metafile; not followed

        Entry entry;
        entry.url = relocate(
            resolveLocation(unescapeEntities(firstCapture(refMatch, 1, 3)), base), base, options);

        const QRegularExpressionMatch titleMatch = titleRe.match(body);
        if (titleMatch.hasMatch())
            entry.title = unescapeEntities(titleMatch.captured(1)).trimmed();

        const QRegularExpressionMatch durationMatch = durationRe.match(body);
        if (durationMatch.hasMatch())
            entry.duration = parseAsxDuration(firstCapture(durationMatch, 1, 3));

        if (acceptEntry(entry, options))
            out.append(entry);
    }

    if (!sawEntry) {
        // Single-stream metafiles routinely omit <ENTRY> entirely, and plenty
        // of files never close it. Fall back to every REF in the document.
        QRegularExpressionMatchIterator refs = asxRefRegex().globalMatch(text);
        while (refs.hasNext()) {
            Entry entry;
            entry.url = relocate(
                resolveLocation(unescapeEntities(firstCapture(refs.next(), 1, 3)), base), base,
                options);
            if (acceptEntry(entry, options))
                out.append(entry);
        }
    }
    return out;
}

QByteArray serializeAsx(const QList<Entry> &entries, const QUrl &base, const WriteOptions &options)
{
    QByteArray output;
    QXmlStreamWriter writer(&output);
    writer.setAutoFormatting(true);

    writer.writeStartElement(QStringLiteral("ASX"));
    writer.writeAttribute(QStringLiteral("version"), QStringLiteral("3.0"));

    for (const Entry &entry : entries) {
        writer.writeStartElement(QStringLiteral("ENTRY"));
        if (!entry.title.isEmpty())
            writer.writeTextElement(QStringLiteral("TITLE"), entry.title);

        writer.writeStartElement(QStringLiteral("REF"));
        writer.writeAttribute(QStringLiteral("HREF"),
                              locationFor(entry.url, base, options, /*asUri=*/true));
        writer.writeEndElement();

        if (entry.duration > 0) {
            writer.writeStartElement(QStringLiteral("DURATION"));
            writer.writeAttribute(
                QStringLiteral("VALUE"),
                QTime::fromMSecsSinceStartOfDay(int(entry.duration % 86400000))
                    .toString(QStringLiteral("HH:mm:ss.zzz")));
            writer.writeEndElement();
        }
        writer.writeEndElement();   // ENTRY
    }

    writer.writeEndElement();       // ASX
    return output;
}

} // namespace

// ---------------------------------------------------------------------------
// Public interface
// ---------------------------------------------------------------------------

std::optional<Format> formatForFile(const QString &path)
{
    const QString suffix = QFileInfo(path).suffix().toLower();
    if (suffix == QLatin1String("m3u"))
        return Format::M3u;
    if (suffix == QLatin1String("m3u8"))
        return Format::M3u8;
    if (suffix == QLatin1String("xspf"))
        return Format::Xspf;
    if (suffix == QLatin1String("b4s"))
        return Format::B4s;
    // .wax/.wvx/.wmx are the audio/video/mixed spellings of the same metafile.
    if (suffix == QLatin1String("asx") || suffix == QLatin1String("wax")
        || suffix == QLatin1String("wvx") || suffix == QLatin1String("wmx"))
        return Format::Asx;
    return std::nullopt;
}

std::optional<Format> formatForContent(const QByteArray &bytes)
{
    const QByteArray head = bytes.left(4096).toLower();
    if (head.contains("xspf.org") || head.contains("<playlist"))
        return Format::Xspf;
    if (head.contains("winampxml"))
        return Format::B4s;
    if (head.contains("<asx"))
        return Format::Asx;
    if (head.contains("#extm3u") || head.contains("#extinf"))
        return Format::M3u8;
    return std::nullopt;
}

QString defaultSuffix(Format format)
{
    switch (format) {
    case Format::M3u:  return QStringLiteral("m3u");
    case Format::M3u8: return QStringLiteral("m3u8");
    case Format::Xspf: return QStringLiteral("xspf");
    case Format::Asx:  return QStringLiteral("asx");
    case Format::B4s:  return QStringLiteral("b4s");
    }
    return QStringLiteral("m3u8");
}

QString openFileFilter()
{
    return QObject::tr("Playlists (*.m3u *.m3u8 *.xspf *.asx *.wax *.wvx *.wmx *.b4s)");
}

QStringList saveFileFilters()
{
    return {
        QObject::tr("M3U8 playlist (*.m3u8)"),
        QObject::tr("M3U playlist (*.m3u)"),
        QObject::tr("XSPF playlist (*.xspf)"),
        QObject::tr("Windows Media metafile (*.asx)"),
        QObject::tr("Winamp playlist (*.b4s)"),
    };
}

QList<Entry> parse(const QByteArray &bytes, Format format, const QUrl &base,
                   const ReadOptions &options)
{
    switch (format) {
    case Format::M3u:
        return parseM3u(stripBom(decodeText(bytes, /*utf8Only=*/false)), base, options);
    case Format::M3u8:
        return parseM3u(stripBom(decodeText(bytes, /*utf8Only=*/true)), base, options);
    case Format::Xspf:
        return parseXspf(bytes, base, options);
    case Format::B4s:
        return parseB4s(bytes, base, options);
    case Format::Asx:
        return parseAsx(stripBom(decodeText(bytes, /*utf8Only=*/false)), base, options);
    }
    return {};
}

QByteArray serialize(const QList<Entry> &entries, Format format, const QUrl &base,
                     const WriteOptions &options)
{
    switch (format) {
    case Format::M3u:
    case Format::M3u8:
        return serializeM3u(entries, base, options);
    case Format::Xspf:
        return serializeXspf(entries, base, options);
    case Format::Asx:
        return serializeAsx(entries, base, options);
    case Format::B4s:
        return serializeB4s(entries, base, options);
    }
    return {};
}

bool load(const QString &path, QList<Entry> *out, QString *error, const ReadOptions &options)
{
    const auto fail = [error](const QString &message) {
        if (error)
            *error = message;
        return false;
    };

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return fail(file.errorString());
    if (file.size() > kMaxPlaylistBytes)
        return fail(QObject::tr("The playlist is too large to read."));

    const QByteArray bytes = file.readAll();
    auto format = formatForFile(path);
    if (!format)
        format = formatForContent(bytes);   // files saved without a suffix
    if (!format)
        return fail(QObject::tr("Unrecognised playlist format."));

    const QUrl base = QUrl::fromLocalFile(QFileInfo(path).absoluteFilePath());
    if (out)
        *out = parse(bytes, *format, base, options);
    return true;
}

bool save(const QString &path, const QList<Entry> &entries, QString *error,
          const WriteOptions &options)
{
    const auto fail = [error](const QString &message) {
        if (error)
            *error = message;
        return false;
    };

    const auto format = formatForFile(path);
    if (!format)
        return fail(QObject::tr("Unrecognised playlist format."));

    const QUrl base = QUrl::fromLocalFile(QFileInfo(path).absoluteFilePath());
    const QByteArray bytes = serialize(entries, *format, base, options);

    // QSaveFile writes to a temporary and renames on commit, so a crash or a
    // full disk leaves the previous playlist intact.
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return fail(file.errorString());
    if (file.write(bytes) != bytes.size()) {
        file.cancelWriting();
        return fail(file.errorString());
    }
    if (!file.commit())
        return fail(file.errorString());
    return true;
}

bool loadInto(Playlist *playlist, const QString &path, QString *error, const ReadOptions &options)
{
    if (!playlist)
        return false;

    QList<Entry> entries;
    if (!load(path, &entries, error, options))
        return false;

    QList<QUrl> urls;
    urls.reserve(entries.size());
    for (const Entry &entry : std::as_const(entries))
        urls.append(entry.url);

    playlist->clear();
    playlist->append(urls);

    // append() only knows the URLs; hand the model the titles and durations the
    // file already told us, so the queue reads properly before anything plays.
    for (int i = 0; i < entries.size(); ++i)
        playlist->updateItemInfo(i, entries.at(i).title, entries.at(i).duration);

    return true;
}

bool saveFrom(const Playlist *playlist, const QString &path, QString *error,
              const WriteOptions &options)
{
    if (!playlist)
        return false;

    QList<Entry> entries;
    entries.reserve(playlist->count());
    for (int i = 0; i < playlist->count(); ++i) {
        const PlaylistItem item = playlist->itemAt(i);
        entries.append(Entry{item.url, item.title, item.duration});
    }
    return save(path, entries, error, options);
}

} // namespace playlistio
