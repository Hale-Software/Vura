//
// Created by halea on 8/5/2026.
//

#ifndef VURA_ADVANCEDSETTINGS_H
#define VURA_ADVANCEDSETTINGS_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui {
class AdvancedSettings;
}
QT_END_NAMESPACE

class AdvancedSettings : public QWidget
{
    Q_OBJECT

public:
    explicit AdvancedSettings(QWidget *parent = nullptr);
    ~AdvancedSettings() override;

private:
    Ui::AdvancedSettings *ui;
};


#endif //VURA_ADVANCEDSETTINGS_H
