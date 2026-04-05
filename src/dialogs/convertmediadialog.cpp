// You may need to build the project (run Qt uic code generator) to get "ui_ConvertMediaDialog.h" resolved

#include "convertmediadialog.h"
#include "ui_ConvertMediaDialog.h"


ConvertMediaDialog::ConvertMediaDialog(QWidget *parent, const QString &initialOption) : QDialog(parent), ui(new Ui::ConvertMediaDialog)
{
    ui->setupUi(this);
    m_initialOption = initialOption;
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
}

ConvertMediaDialog::~ConvertMediaDialog()
{
    delete ui;
}
