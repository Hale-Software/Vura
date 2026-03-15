#ifndef HEDITMARKER_H
#define HEDITMARKER_H

#include <QDialog>
#include <QString>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>


QT_BEGIN_NAMESPACE

namespace Ui
{
    class EditMarkerDialog;
}

QT_END_NAMESPACE

class EditMarkerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditMarkerDialog(QWidget *parent = nullptr);
    ~EditMarkerDialog() override;

public slots:
    void setMarkerInfo(const QString &markerName, const double &markerTime, const QString &markerType);
    void cancelButton_Clicked();
    void okButton_Clicked();
    void deleteButton_Clicked();
    void nameTextBox_TextChanged(const QString &text);
    void timeTextBox_TextChanged(const QString &text);
    void markerTypeComboBox_CurrentIndexChanged(int index);

signals:
    void markerSaved(QString markerName, double markerTime, QString markerType);
    void markerDeleted(double markerTime);

private:
    Ui::EditMarkerDialog *ui;
    QString m_markerName;
    double m_markerTime;
    QString m_markerType;

};


#endif //EDITMARKER_H
