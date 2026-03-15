#ifndef APPLICATION_H
#define APPLICATION_H

#include "actionmanager.h"

#include <QApplication>

#include <memory>

class Application : public QApplication
{
    Q_OBJECT

public:
    Application(int &argc, char **argv);
    ~Application() override = default;

    ActionManager *actionManager() const;

private:
    std::unique_ptr<ActionManager> m_actionManager;
};

#endif
