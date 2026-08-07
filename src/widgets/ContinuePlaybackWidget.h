#pragma once

#include <QWidget>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QString>
#include <QTime>

QT_BEGIN_NAMESPACE
namespace Ui {
class ContinuePlaybackWidget;
}
QT_END_NAMESPACE


class ContinuePlaybackWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContinuePlaybackWidget(qint64 savedPosition, QWidget *parent = nullptr);
    ~ContinuePlaybackWidget() override;

signals:
    void continuePlayback(qint64 savedPosition);
    void closeWidget();

private slots:
    void continuePlaybackClicked();
    void closeWidgetClicked();

private:
    Ui::ContinuePlaybackWidget *ui;

    qint64 m_savedPosition;

};
