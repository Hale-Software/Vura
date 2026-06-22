//
// Created by halea on 6/21/2026.
//

#ifndef VURA_ERRORDIALOG_H
#define VURA_ERRORDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>

#include <libvura/ErrorService.h>
#include <libvura/ErrorTypes.h>


QT_BEGIN_NAMESPACE
namespace Ui {
class ErrorDialog;
}
QT_END_NAMESPACE

class ErrorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ErrorDialog(QWidget *parent = nullptr);
    ~ErrorDialog() override;


public slots:
    void showError(const PlayerError& error);
    void closeButton_Clicked();
    void errorActionButton_Clicked();

private:
    Ui::ErrorDialog *ui;
    PlayerError m_error;

};


#endif //VURA_ERRORDIALOG_H
