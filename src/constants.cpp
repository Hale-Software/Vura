/*******************************************************************************
     Copyright (c) 2026.  by Andrew Hale <halea2196@gmail.com>

     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "constants.h"


QString _applicationDebugFolder = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

QString _videoFileExtensions = R"(
*.3g2 *.3gp *.3gp2 *.3gpp *.amv *.asf *.avi *.bik *.bin *.crf *.dav *.divx *.drc *.dv *.dvr-ms *.evo *.f4v *.flv
 *.gvi *.gxf *.iso *.m1v *.m2v *.m2t *.m2ts *.m4v *.mkv *.mov *.mp2 *.mp2v *.mp4 *.mp4v *.mpe *.mpeg *.mpeg1
 *.mpeg2 *.mpeg4 *.mpg *.mpv2 *.mts *.mtv *.mxf *.mxg *.nsv *.nuv *.ogg *.ogm *.ogv *.ogx *.ps *.rec *.rm *.rmvb
 *.rpl *.thp *.tod *.tp *.ts *.tts *.txd *.vob *.vprj *.vro *.webm *.wm *.wmv *.wtv *.xesc
)";

QString _audioFileExtensions = R"(
*.3ga *.669 *.a52 *.aac * *.ac3 .adt *.adts *.aif *.aifc *.aiff *.amb *.amr *.aob *.ape *.au *.awb *.caf *.dts *.flac
 *.it *.kar *.m4a *.m4b *.m4p *.m5p *.mid *.mka *.mlp *.mod *.mpa *.mp1 *.mp2 *.mp3 *.mpc *.mpga *.mus *.oga *.ogg
 *.oma *.opus *.qcp *.ra *.rmi *.s3m *.sid *.spx *.tak *.thd *.tta *.voc *.vqf *.w64 *.wav *.wma *.wv *.xa *.xm
)";

QString _applicationFileExtensions = R"(
*.vhk *.vprj *vvm
)";

QString _playlistFileExtensions = R"(
*.asx *.b4s *.cue *.hlist *.ifo *.m3u *.m3u8 *.pls *.ram *.rar *.sdp *.vlc *.vpl *.xspf *.wax *.wvx *.zip *.conf
)";


namespace constants {

    extern const QString ApplicationDebugFolder {
        _applicationDebugFolder + "/vura-debug"
    };

    extern const QString MediaFileExtensions {
        "Media Files ("
        + _videoFileExtensions + " "
        + _audioFileExtensions + " "
        + _applicationFileExtensions + " "
        + _playlistFileExtensions
        + ")"
    };

    extern const QString VideoFileExtensions {
        "Video Files ("
        + _videoFileExtensions
        + ")"
    };

    extern const QString AudioFileExtensions {
        "Audio Files ("
        + _audioFileExtensions
        + ")"
    };

    extern const QString ApplicationFileExtensions {
        "Application Files ("
        + _applicationFileExtensions
        + ")"
    };

    extern const QString PlaylistFileExtensions {
        "Playlist Files ("
        + _playlistFileExtensions
        + ")"
    };

}
