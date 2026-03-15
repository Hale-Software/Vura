#include "main.h"

#include <config.h>

VuraApp::VuraApp(int &argc, char **argv) : QApplication(argc, argv) {}

VuraApp::~VuraApp() {}

void VuraApp::AppInit(int argc, char* argv[])
{
    bool openedWithFile = false;
    QString fileName;

    QCoreApplication::setApplicationName("vura");
    QCoreApplication::setOrganizationName("Hale Software");
    QCoreApplication::setApplicationVersion(VURA_VERSION_CANONICAL);

    // Check if file was opened with application
    if (argc > 1) {
        openedWithFile = true;
        fileName = QString::fromLocal8Bit(argv[1]);
    }

    if (useTestWindow) {
        testWindow = new TestWindow();
        testWindow->setAttribute(Qt::WA_DeleteOnClose, true);
        testWindow->show();
    } else {
        mainWindow = new MainWindow();
        mainWindow->setAttribute(Qt::WA_DeleteOnClose, true);
        mainWindow->setWindowTitle("Vura " + QString::fromUtf8(VURA_VERSION_STRING));
        connect(mainWindow, SIGNAL(destroyed()), this, SLOT(quit()));
        mainWindow->show();
        if (openedWithFile) {
            mainWindow->openedWithFile(fileName);
        }
    }
}

static int run_program(int argc, char* argv[])
{
    int ret = -1;

    VuraApp program(argc, argv);
    program.AppInit(argc, argv);
    ret = program.exec();

    return ret;
}

int main(int argc, char *argv[])
{
    int ret = run_program(argc, argv);
    return ret;
}
