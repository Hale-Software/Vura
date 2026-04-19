#include "vura-helpers.h"


void VuraHelpers::simulateApplicationCrash()
{
    int *ptr = nullptr;
    *ptr = 42; // Accessing address 0 causes a SIGSEGV/Access Violation
}

bool VuraHelpers::fileExists(const QString &fileName)
{
    QFileInfo file(fileName);
    if (file.exists() && file.isFile()) {
        return true;
    }

    return false;
}

const char *VuraHelpers::QStringToChar(const QString &text)
{
    QByteArray byteArray = text.toUtf8();
    return byteArray.constData();
}

int VuraHelpers::qint64ToInt(const qint64 &number)
{
    return static_cast<int>(number);
}
