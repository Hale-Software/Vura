#include "editmarker.h"
#include "../forms/ui_editmarker.h"


EditMarkerDialog::EditMarkerDialog(QWidget *parent) : QDialog(parent), ui(new Ui::EditMarkerDialog)
{
    ui->setupUi(this);
}

EditMarkerDialog::~EditMarkerDialog()
{
    delete ui;
}

void EditMarkerDialog::setMarkerInfo(const QString &markerName, const double &markerTime, const QString &markerType)
{
    m_markerName = markerName;
    m_markerTime = markerTime;
    m_markerType = markerType;

    if (m_markerType.isEmpty())
    {
        ui->typeComboBox->setCurrentIndex(0);
    } else if (m_markerType == "marker")
    {
        ui->typeComboBox->setCurrentIndex(0);
    } else if (m_markerType == "scene")
    {
        ui->typeComboBox->setCurrentIndex(1);
    } else if (m_markerType == "cumshot")
    {
        ui->typeComboBox->setCurrentIndex(2);
    } else if (m_markerType == "strip")
    {
        ui->typeComboBox->setCurrentIndex(3);
    } else
    {
        ui->typeComboBox->setCurrentIndex(0);
    }

    ui->timeTextBox->setText(QString::number(m_markerTime, 'f', 2));
    ui->nameTextBox->setText(m_markerName);
}

void EditMarkerDialog::cancelButton_Clicked()
{
    this->close();
}

void EditMarkerDialog::okButton_Clicked()
{
    emit markerSaved(m_markerName, m_markerTime, m_markerType);
    this->close();
}

void EditMarkerDialog::deleteButton_Clicked()
{
    QMessageBox::StandardButton response;
    response = QMessageBox::question(
        this,
        "Confirm Delete",
        "Are you sure you want to delete this marker?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (response == QMessageBox::Yes)
    {
        emit markerDeleted(m_markerTime);
        this->close();
    } else
    {

    }
}

void EditMarkerDialog::nameTextBox_TextChanged(const QString &text)
{
    m_markerName = text;
    ui->okButton->setEnabled(true);
}

void EditMarkerDialog::timeTextBox_TextChanged(const QString &text)
{
    m_markerTime = text.toDouble();
    ui->okButton->setEnabled(true);
}

void EditMarkerDialog::markerTypeComboBox_CurrentIndexChanged(int index)
{
    m_markerType = ui->typeComboBox->itemText(index);
    ui->okButton->setEnabled(true);
}
