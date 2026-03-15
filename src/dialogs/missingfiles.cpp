#include "missingfiles.h"
#include "../forms/ui_missingfiles.h"


MissingFilesDialog::MissingFilesDialog(QWidget *parent) : QDialog(parent), ui(new Ui::MissingFilesDialog)
{
    ui->setupUi(this);
}

MissingFilesDialog::~MissingFilesDialog()
{
    delete ui;
}