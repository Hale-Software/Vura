#include "whatsnew.h"
#include "../forms/ui_whatsnew.h"


WhatsNewDialog::WhatsNewDialog(QWidget *parent) : QDialog(parent), ui(new Ui::WhatsNewDialog)
{
    ui->setupUi(this);
}

WhatsNewDialog::~WhatsNewDialog()
{
    delete ui;
}