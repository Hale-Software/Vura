#pragma once

#include <QWidget>
#include <QSettings>
#include <QDebug>


QT_BEGIN_NAMESPACE
namespace Ui {
class AssociationsSettings;
}
QT_END_NAMESPACE

class AssociationsSettings : public QWidget
{
    Q_OBJECT

public:
    explicit AssociationsSettings(QWidget *parent = nullptr);
    ~AssociationsSettings() override;

    bool unsavedChanges();

signals:
    void settingsChanged();
    void requiresRestart();

public slots:
    void resetToDefaults();
    void saveSettings();

private:
    Ui::AssociationsSettings *ui;
    bool m_unsavedChanges = false;

};
