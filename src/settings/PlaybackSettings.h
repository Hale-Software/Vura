#pragma once

#include <QWidget>
#include <QSettings>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDebug>


QT_BEGIN_NAMESPACE
namespace Ui {
class PlaybackSettings;
}
QT_END_NAMESPACE

class PlaybackSettings : public QWidget
{
    Q_OBJECT

public:
    explicit PlaybackSettings(QWidget *parent = nullptr);
    ~PlaybackSettings() override;

    bool unsavedChanges();

signals:
    void settingsChanged();
    void requiresRestart();

public slots:
    void resetToDefaults();
    void saveSettings();

private:
    Ui::PlaybackSettings *ui;
    bool m_unsavedChanges = false;

};
