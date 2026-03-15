#include "about.h"
#include "../forms/ui_about.h"

#include <config.h>

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->version->setText(QString::fromStdString(VURA_VERSION_STRING));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
