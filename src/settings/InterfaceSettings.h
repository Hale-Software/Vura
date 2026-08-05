//
// Created by halea on 8/5/2026.
//

#ifndef VURA_INTERFACESETTINGS_H
#define VURA_INTERFACESETTINGS_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui {
class InterfaceSettings;
}
QT_END_NAMESPACE

class InterfaceSettings : public QWidget
{
    Q_OBJECT

public:
    explicit InterfaceSettings(QWidget *parent = nullptr);
    ~InterfaceSettings() override;

private:
    Ui::InterfaceSettings *ui;
};


#endif //VURA_INTERFACESETTINGS_H
