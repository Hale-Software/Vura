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
#include "platform.h"


const char* _videoFileExtensions = "*.3g2 *.3gp *.3gp2 *.3gpp *.amv *.asf *.avi *.bik *.bin *.crf *.dav *.divx *.drc \n"
									  "*.dv *.dvr-ms *.evo *.f4v *.flv *.gvi *.gxf *.iso *.m1v *.m2v *.m2t *.m2ts *.m4v \n"
									  "*.mkv *.mov *.mp2 *.mp2v *.mp4 *.mp4v *.mpe *.mpeg *.mpeg1 *.mpeg2 *.mpeg4 *.mpg \n"
									  "*.mpv2 *.mts *.mtv *.mxf *.mxg *.nsv *.nuv *.ogg *.ogm *.ogv *.ogx *.ps *.rec \n"
									  "*.rm *.rmvb *.rpl *.thp *.tod *.tp *.ts *.tts *.txd *.vob *.vprj *.vro *.webm \n"
									  "*.wm *.wmv *.wtv *.xesc";

const char* _audioFileExtensions = "*.3ga *.669 *.a52 *.aac * *.ac3 .adt *.adts *.aif *.aifc *.aiff *.amb *.amr \n"
									  "*.aob *.ape *.au *.awb *.caf *.dts *.flac *.it *.kar *.m4a *.m4b *.m4p *.m5p \n"
									  "*.mid *.mka *.mlp *.mod *.mpa *.mp1 *.mp2 *.mp3 *.mpc *.mpga *.mus *.oga *.ogg \n"
									  "*.oma *.opus *.qcp *.ra *.rmi *.s3m *.sid *.spx *.tak *.thd *.tta *.voc *.vqf \n"
									  "*.w64 *.wav *.wma *.wv *.xa *.xm";


const char* _libapplicationFileExtensions = "*.vhk *.vprj *vvm";


const char* _libplaylistFileExtensions = "*.asx *.b4s *.cue *.hlist *.ifo *.m3u *.m3u8 *.pls *.ram *.rar *.sdp *.vlc \n"
										 "*.vpl *.xspf *.wax *.wvx *.zip *.conf";


char *libApplicationDebugFolder()
{
	char *home = os_get_debug_path();
    _libapplicationDebugFolder + "/vura-debug"
}

char *libMediaFileExtensions {
    "Media Files ("
    + _libvideoFileExtensions + " "
    + _libaudioFileExtensions + " "
    + _libapplicationFileExtensions + " "
    + _libplaylistFileExtensions
    + ")"
};

char *libVideoFileExtensions {
    "Video Files ("
    + _libvideoFileExtensions
    + ")"
};

char *libAudioFileExtensions {
    "Audio Files ("
    + _libaudioFileExtensions
    + ")"
};

char *libApplicationFileExtensions {
    "Application Files ("
    + _libapplicationFileExtensions
    + ")"
};

char *libPlaylistFileExtensions {
    "Playlist Files ("
    + _libplaylistFileExtensions
    + ")"
};
