#pragma once

#include <QWidget>
#include <QSettings>
#include <QDebug>


QT_BEGIN_NAMESPACE
namespace Ui {
class PlaylistSettings;
}
QT_END_NAMESPACE

class PlaylistSettings : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistSettings(QWidget *parent = nullptr);
    ~PlaylistSettings() override;

    bool unsavedChanges();

signals:
    void settingsChanged();
    void requiresRestart();

public slots:
    void resetToDefaults();
    void saveSettings();

private:
    Ui::PlaylistSettings *ui;
    bool m_unsavedChanges = false;

};
