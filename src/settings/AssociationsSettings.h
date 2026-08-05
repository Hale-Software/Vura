//
// Created by halea on 8/5/2026.
//

#ifndef VURA_ASSOCIATIONSSETTINGS_H
#define VURA_ASSOCIATIONSSETTINGS_H

#include <QWidget>


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

private:
    Ui::AssociationsSettings *ui;
};


#endif //VURA_ASSOCIATIONSSETTINGS_H
