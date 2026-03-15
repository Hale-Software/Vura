#ifndef HPLAYER_QT_WRAPPERS_H
#define HPLAYER_QT_WRAPPERS_H

#include <QApplication>
#include <QMessageBox>
#include <QWidget>
#include <QWindow>
#include <QThread>
#include <QObject>

#define QT_UTF8(str) QString::fromUtf8(str, -1)
#define QT_TO_UTF8(str) str.toUtf8().constData()
#define MAX_LABEL_LENGTH 80

class QDataStream;
class QComboBox;
class QWidget;
class QLayout;
class QString;
class QLabel;
class QToolBar;

class VuraMessageBox : public QObject {
    Q_OBJECT

public:
    static QMessageBox::StandardButton question(
            QWidget *parent, const QString &title, const QString &text,
            QMessageBox::StandardButtons buttons = QMessageBox::StandardButtons(QMessageBox::Yes | QMessageBox::No),
            QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

    static void information(QWidget *parent, const QString &title, const QString &text);
    static void warning(QWidget *parent, const QString &title, const QString &text, bool enableRichText = false);
    static void critical(QWidget *parent, const QString &title, const QString &text);

};

void VuraErrorBox(QWidget *parent, const char *msg, ...);


#endif //HPLAYER_QT_WRAPPERS_H
