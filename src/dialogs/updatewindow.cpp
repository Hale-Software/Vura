#include "updatewindow.h"
#include "ui_updatewindow.h"


UpdateDialog::UpdateDialog(QWidget *parent) : QDialog(parent), ui(new Ui::UpdateDialog)
{
    ui->setupUi(this);
}

UpdateDialog::~UpdateDialog()
{
    delete ui;
}