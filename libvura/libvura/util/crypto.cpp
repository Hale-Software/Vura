#include "crypto.h"


QString Crypto::fingerprintFile(const QString &path, qint64 sampleSize)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return {};

    QCryptographicHash hash(QCryptographicHash::Sha1); // or Md5 if you want raw speed
    qint64 fileSize = file.size();

    // Mix in file size — cheap collision resistance
    hash.addData(reinterpret_cast<const char*>(&fileSize), sizeof(fileSize));

    // Sample beginning
    hash.addData(file.read(sampleSize));

    // Sample middle
    if (fileSize > sampleSize * 3) {
        file.seek(fileSize / 2);
        hash.addData(file.read(sampleSize));
    }

    // Sample end
    if (fileSize > sampleSize * 2) {
        file.seek(qMax<qint64>(0, fileSize - sampleSize));
        hash.addData(file.read(sampleSize));
    }

    return QString::fromUtf8(hash.result());
}
