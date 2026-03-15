#ifndef HPLAYER_LOGVIEWER_H
#define HPLAYER_LOGVIEWER_H

#include <QDialog>
#include <QPushButton>
#include <QTextBrowser>
#include <QComboBox>
#include <QCheckBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QString>

#include "../utility/logger.h"
#include "../utility/qt-wrappers.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class LogViewer;
}

QT_END_NAMESPACE

class LogViewer : public QDialog {
    Q_OBJECT

public:
    explicit LogViewer(QWidget *parent = nullptr);
    ~LogViewer() override;

public slots:
    void message(QString message);

private slots:
    //void AddLine(int type, const QString &text);
    void openButton_Clicked();
    void clearButton_Clicked();
    void closeButton_Clicked();
    void verbosityIndexChanged(int index);
    void simplify_Clicked();

private:
    Ui::LogViewer *ui;
    int m_verbosity = 0;
    bool m_simplify = true;
    QString m_currentLogFile;
    QString m_openedLogFile;

    void refreshMessages();
    void messageFormatter(QString message);

};


#endif //HPLAYER_LOGVIEWER_H
