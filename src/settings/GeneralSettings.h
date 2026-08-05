//
// Created by halea on 8/5/2026.
//

#ifndef VURA_GENERALSETTINGS_H
#define VURA_GENERALSETTINGS_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui {
class GeneralSettings;
}
QT_END_NAMESPACE

class GeneralSettings : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralSettings(QWidget *parent = nullptr);
    ~GeneralSettings() override;

private:
    Ui::GeneralSettings *ui;
};


#endif //VURA_GENERALSETTINGS_H
