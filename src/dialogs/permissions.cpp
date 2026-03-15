#include "permissions.h"
#include "../forms/ui_permissions.h"


PermissionsDialog::PermissionsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::PermissionsDialog)
{
    ui->setupUi(this);
}

PermissionsDialog::~PermissionsDialog()
{
    delete ui;
}