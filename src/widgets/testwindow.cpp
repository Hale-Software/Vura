// You may need to build the project (run Qt uic code generator) to get "ui_TestWindow.h" resolved

#include "testwindow.h"
#include "ui_testwindow.h"


static Logger* globalRedirector = nullptr;

TestWindow::TestWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::TestWindow)
{
    ui->setupUi(this);

    qInstallMessageHandler(Logger::messageHandler);
    globalRedirector = Logger::instance();

    m_menuBar = new MenuBar(this);
    this->setMenuBar(m_menuBar);

    m_videoPlayer = new VideoPlayer(this);
    ui->verticalLayout->addWidget(m_videoPlayer);
    QUrl url = QUrl::fromLocalFile("C:\\Users\\halea\\Videos\\Extra\\faphouse.com-captures-latin-hotel-maid-sucking-my-dick-p720.mp4");
    m_videoPlayer->setSource(url);

}

TestWindow::~TestWindow()
{
    delete ui;
}
