#pragma once

#include <QApplication>
#include <QCoreApplication>
#include <QPointer>
#include <QString>
#include <QMessageBox>

#include <qBreakpad/handler/QBreakpadHandler.h>

#include "widgets/mainwindow.h"
#include "widgets/testwindow.h"


class VuraApp : public QApplication {
    Q_OBJECT

    friend class MainWindow;
    friend class TestWindow;

private:
    QPointer<MainWindow> mainWindow;
    QPointer<TestWindow> testWindow;

    bool useTestWindow = false;

public:
    VuraApp(int& argc, char** argv);
    ~VuraApp();

    void AppInit(int argc, char* argv[]);

};
