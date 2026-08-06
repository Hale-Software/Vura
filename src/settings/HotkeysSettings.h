#pragma once

#include <QWidget>
#include <QSettings>
#include <QDebug>


QT_BEGIN_NAMESPACE
namespace Ui {
class HotkeysSettings;
}
QT_END_NAMESPACE

class HotkeysSettings : public QWidget
{
    Q_OBJECT

public:
    explicit HotkeysSettings(QWidget *parent = nullptr);
    ~HotkeysSettings() override;

    bool unsavedChanges();

signals:
    void settingsChanged();
    void requiresRestart();

public slots:
    void resetToDefaults();
    void saveSettings();

private:
    Ui::HotkeysSettings *ui;
    bool m_unsavedChanges = false;

};
