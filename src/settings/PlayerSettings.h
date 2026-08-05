//
// Created by halea on 8/5/2026.
//

#ifndef VURA_PLAYERSETTINGS_H
#define VURA_PLAYERSETTINGS_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui {
class PlayerSettings;
}
QT_END_NAMESPACE

class PlayerSettings : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerSettings(QWidget *parent = nullptr);
    ~PlayerSettings() override;

private:
    Ui::PlayerSettings *ui;
};


#endif //VURA_PLAYERSETTINGS_H
