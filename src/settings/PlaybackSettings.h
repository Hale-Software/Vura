//
// Created by halea on 8/5/2026.
//

#ifndef VURA_PLAYBACKSETTINGS_H
#define VURA_PLAYBACKSETTINGS_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui {
class PlaybackSettings;
}
QT_END_NAMESPACE

class PlaybackSettings : public QWidget
{
    Q_OBJECT

public:
    explicit PlaybackSettings(QWidget *parent = nullptr);
    ~PlaybackSettings() override;

private:
    Ui::PlaybackSettings *ui;
};


#endif //VURA_PLAYBACKSETTINGS_H
