#ifndef HPLAYER_TESTWINDOW_H
#define HPLAYER_TESTWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QDebug>

#include "../components/menubar.h"
#include "../components/videoplayer.h"
#include "../utility/logger.h"


QT_BEGIN_NAMESPACE

namespace Ui {
    class TestWindow;
}

QT_END_NAMESPACE

class TestWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit TestWindow(QWidget *parent = nullptr);

    ~TestWindow() override;

private:
    Ui::TestWindow *ui;
    Logger* m_hLogger;
    MenuBar *m_menuBar = nullptr;
    VideoPlayer *m_videoPlayer = nullptr;

};


#endif //HPLAYER_TESTWINDOW_H
