#pragma once

#include <QApplication>

#include "VuraMainWindow.h"


class VuraApplication : public QApplication
{
public:
    VuraApplication(int &argc, char **argv);

    void setWindow(VuraMainWindow *window);
    void setTheme(const int &theme);

private:
    VuraMainWindow *m_window = nullptr;

};
