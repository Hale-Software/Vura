//
// Created by halea on 8/5/2026.
//

#ifndef VURA_PLAYLISTSETTINGS_H
#define VURA_PLAYLISTSETTINGS_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui {
class PlaylistSettings;
}
QT_END_NAMESPACE

class PlaylistSettings : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistSettings(QWidget *parent = nullptr);
    ~PlaylistSettings() override;

private:
    Ui::PlaylistSettings *ui;
};


#endif //VURA_PLAYLISTSETTINGS_H
