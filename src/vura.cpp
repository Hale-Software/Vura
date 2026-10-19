#include "vura.h"

#include <QFile>
#include <QMessageBox>


VuraApplication::VuraApplication(int &argc, char **argv)
    : QApplication(argc, argv) {}

void VuraApplication::setWindow(VuraMainWindow *window)
{
    m_window = window;
}

void VuraApplication::setTheme(const int &theme)
{
    bool themeChanged = false;

    switch (theme) {
        case 0:
            // Theme
            break;

        case 1:
            // Theme
            break;

        case 2:
            // Theme
            break;

        case 3:
            // Nova Theme
            QFile style(QStringLiteral(":/styles/nova.qss"));
            if (style.open(QIODevice::ReadOnly | QIODevice::Text)) {
                this->setStyleSheet(QString::fromUtf8(style.readAll()));
                themeChanged = true;
            }
            break;
    }

    if (!themeChanged)
        QMessageBox::critical(nullptr, tr("Vura Error"), tr("Failed to load Vura theme."));
}
