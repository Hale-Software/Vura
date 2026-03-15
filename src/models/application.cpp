#include "application.h"

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv)
{
    m_actionManager = std::make_unique<ActionManager>();
}

ActionManager *Application::actionManager() const
{
    return m_actionManager.get();
}
