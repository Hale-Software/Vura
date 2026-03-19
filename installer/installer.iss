#define MyAppName "Vura"
#define MyAppVersion '0.0.0'
#define MyAppPublisher "Hale Software LLC"
#define MyAppURL "https://hale-software.github.io/"
#define MyAppExeName "vura64.exe"

[Setup]
AppId={{1492C056-B050-4F8D-A88F-D280D70B79F8}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
UninstallDisplayIcon={app}\{#MyAppExeName}
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
ChangesAssociations=yes
DisableProgramGroupPage=yes
LicenseFile=D:\a\Vura\Vura\build\release\src\data\license\license.txt
OutputDir=D:\a\Vura\Vura\installer
OutputBaseFilename=D:\a\Vura\Vura\installer\vura-0.0.0-win64
SetupIconFile=D:\a\Vura\Vura\installer\vura.ico
SolidCompression=yes
WizardStyle=modern
DisableWelcomePage=no

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Types]
Name: "full"; Description: "Full Installation"
Name: "custom"; Description: "Custom Installation"; Flags: iscustom

[Components]
Name: "player"; Description: "Media Player"; Types: full custom; Flags: fixed
Name: "startMenuShortcut"; Description: "Start Menu Shortcut"; Types: full custom;
Name: "desktopShortcut"; Description: "Desktop Shortcut"; Types: full custom;
Name: "discsPlayback"; Description: "Discs Playback"; Types: full custom;


Name: "fileTypeAssociations"; Description: "File Type Associations"; Types: full custom;

Name: "fileTypeAssociations\audioFiles"; Description: "Audio Files"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\3ga"; Description: ".3ga"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\669"; Description: ".669"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\a52"; Description: ".a52"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\aac"; Description: ".aac"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\ac3"; Description: ".ac3"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\adt"; Description: ".adt"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\adts"; Description: ".adts"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\aif"; Description: ".aif"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\aifc"; Description: ".aifc"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\aiff"; Description: ".aiff"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\au"; Description: ".au"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\amr"; Description: ".amr"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\aob"; Description: ".aob"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\ape"; Description: ".ape"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\caf"; Description: ".caf"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\cda"; Description: ".cda"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\dts"; Description: ".dts"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\flac"; Description: ".flac"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\it"; Description: ".it"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\m4a"; Description: ".m4a"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\m4p"; Description: ".m4p"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\mlp"; Description: ".mlp"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\mod"; Description: ".mod"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\mp1"; Description: ".mp1"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\mp2"; Description: ".mp2"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\mp3"; Description: ".mp3"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\mpc"; Description: ".mpc"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\mpga"; Description: ".mpga"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\oga"; Description: ".oga"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\oma"; Description: ".oma"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\opus"; Description: ".opus"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\qcp"; Description: ".qcp"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\ra"; Description: ".ra"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\rmi"; Description: ".rmi"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\snd"; Description: ".snd"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\s3m"; Description: ".s3m"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\spx"; Description: ".spx"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\tta"; Description: ".tta"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\voc"; Description: ".voc"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\vqf"; Description: ".vqf"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\w64"; Description: ".w64"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\wav"; Description: ".wav"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\wma"; Description: ".wma"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\wv"; Description: ".wv"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\xa"; Description: ".xa"; Types: full custom;
Name: "fileTypeAssociations\audioFiles\xm"; Description: ".xm"; Types: full custom;

Name: "fileTypeAssociations\videoFiles"; Description: "Video Files"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\3g2"; Description: ".3g2"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\3gp"; Description: ".3gp"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\3gp2"; Description: ".3gp2"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\3gpp"; Description: ".3gpp"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\amv"; Description: ".amv"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\asf"; Description: ".asf"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\avi"; Description: ".avi"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\bik"; Description: ".bik"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\dav"; Description: ".dav"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\divx"; Description: ".divx"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\drc"; Description: ".drc"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\dv"; Description: ".dv"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\dvrms"; Description: ".dvr-ms"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\evo"; Description: ".evo"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\f4v"; Description: ".f4v"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\flv"; Description: ".flv"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\gvi"; Description: ".gvi"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\gxf"; Description: ".gxf"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\m1v"; Description: ".m1v"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\m2t"; Description: ".m2t"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\m2v"; Description: ".m2v"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\m2ts"; Description: ".m2ts"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\m4v"; Description: ".m4v"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\mkv"; Description: ".mkv"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\mov"; Description: ".mov"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\mp2v"; Description: ".mp2v"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\mp4"; Description: ".mp4"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\mp4v"; Description: ".mp4v"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\mpa"; Description: ".mpa"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\mpe"; Description: ".mpe"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\mpeg"; Description: ".mpeg"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\mpeg1"; Description: ".mpeg1"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\mpeg2"; Description: ".mpeg2"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\mpeg4"; Description: ".mpeg4"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\mpg"; Description: ".mpg"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\mpv2"; Description: ".mpv2"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\mts"; Description: ".mts"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\mtv"; Description: ".mtv"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\mxf"; Description: ".mxf"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\nsv"; Description: ".nsv"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\nuv"; Description: ".nuv"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\ogg"; Description: ".ogg"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\ogm"; Description: ".ogm"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\ogx"; Description: ".ogx"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\ogv"; Description: ".ogv"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\rec"; Description: ".rec"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\rm"; Description: ".rm"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\rmvb"; Description: ".rmvb"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\rpl"; Description: ".rpl"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\thp"; Description: ".thp"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\tod"; Description: ".tod"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\tp"; Description: ".tp"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\ts"; Description: ".ts"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\tts"; Description: ".tts"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\vob"; Description: ".vob"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\vprj"; Description: ".vprj"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\vro"; Description: ".vro"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\webm"; Description: ".webm"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\wmv"; Description: ".wmv"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\wtv"; Description: ".wtv"; Types: full custom;
Name: "fileTypeAssociations\videoFiles\xesc"; Description: ".xesc"; Types: full custom;

Name: "fileTypeAssociations\otherFiles"; Description: "Other"; Types: full custom;
Name: "fileTypeAssociations\otherFiles\b4s"; Description: ".b4s"; Types: full custom;
Name: "fileTypeAssociations\otherFiles\hkey"; Description: ".hkey"; Types: full custom;
Name: "fileTypeAssociations\otherFiles\hmrk"; Description: ".hmrk"; Types: full custom;
Name: "fileTypeAssociations\otherFiles\hpl"; Description: ".hpl"; Types: full custom;
Name: "fileTypeAssociations\otherFiles\hprj"; Description: ".hprj"; Types: full custom;
Name: "fileTypeAssociations\otherFiles\m3u"; Description: ".m3u"; Types: full custom;
Name: "fileTypeAssociations\otherFiles\m3u8"; Description: ".m3u8"; Types: full custom;
Name: "fileTypeAssociations\otherFiles\pls"; Description: ".pls"; Types: full custom;
Name: "fileTypeAssociations\otherFiles\wpl"; Description: ".wpl"; Types: full custom;
Name: "fileTypeAssociations\otherFiles\vpl"; Description: ".vpl"; Types: full custom;
Name: "fileTypeAssociations\otherFiles\xspf"; Description: ".xspf"; Types: full custom;
;Name: "fileTypeAssociations\otherFiles\"; Description: "."; Types: full custom;



[Files]
Source: "D:\a\Vura\Vura\build\release\src\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\a\Vura\Vura\build\release\src\avcodec-61.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\a\Vura\Vura\build\release\src\avformat-61.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\a\Vura\Vura\build\release\src\avutil-59.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\a\Vura\Vura\build\release\src\config.h"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\a\Vura\Vura\build\release\src\D3Dcompiler_47.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\a\Vura\Vura\build\release\src\dxcompiler.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\a\Vura\Vura\build\release\src\vura.rc"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\a\Vura\Vura\build\release\src\libgcc_s_seh-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\a\Vura\Vura\build\release\src\libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\a\Vura\Vura\build\release\src\libwinpthread-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\a\Vura\Vura\build\release\src\opengl32sw.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\a\Vura\Vura\build\release\src\Qt6Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\a\Vura\Vura\build\release\src\Qt6Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\a\Vura\Vura\build\release\src\Qt6Multimedia.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\a\Vura\Vura\build\release\src\Qt6MultimediaWidgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\a\Vura\Vura\build\release\src\Qt6Network.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\a\Vura\Vura\build\release\src\Qt6Svg.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\a\Vura\Vura\build\release\src\Qt6Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\a\Vura\Vura\build\release\src\swresample-5.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\a\Vura\Vura\build\release\src\swscale-8.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\a\Vura\Vura\build\release\src\data\*"; DestDir: "{app}\data"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "D:\a\Vura\Vura\build\release\src\generic\*"; DestDir: "{app}\generic"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "D:\a\Vura\Vura\build\release\src\iconengines\*"; DestDir: "{app}\iconengines"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "D:\a\Vura\Vura\build\release\src\imageformats\*"; DestDir: "{app}\imageformats"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "D:\a\Vura\Vura\build\release\src\multimedia\*"; DestDir: "{app}\multimedia"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "D:\a\Vura\Vura\build\release\src\networkinformation\*"; DestDir: "{app}\networkinformation"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "D:\a\Vura\Vura\build\release\src\platforms\*"; DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "D:\a\Vura\Vura\build\release\src\styles\*"; DestDir: "{app}\styles"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "D:\a\Vura\Vura\build\release\src\tls\*"; DestDir: "{app}\tls"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "D:\a\Vura\Vura\build\release\src\translations\*"; DestDir: "{app}\translations"; Flags: ignoreversion recursesubdirs createallsubdirs

[Registry]
; Audio File Associations
Root: HKA; Subkey: "Software\Classes\.3ga\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.3ga"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\3ga
Root: HKA; Subkey: "Software\Classes\.669\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.669"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\669
Root: HKA; Subkey: "Software\Classes\.a52\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.a52"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\a52
Root: HKA; Subkey: "Software\Classes\.aac\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.aac"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\aac
Root: HKA; Subkey: "Software\Classes\.ac3\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.ac3"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\ac3
Root: HKA; Subkey: "Software\Classes\.adt\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.adt"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\adt
Root: HKA; Subkey: "Software\Classes\.adts\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.adts"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\adts
Root: HKA; Subkey: "Software\Classes\.aif\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.aif"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\aif
Root: HKA; Subkey: "Software\Classes\.aifc\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.aifc"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\aifc
Root: HKA; Subkey: "Software\Classes\.aiff\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.aiff"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\aiff
Root: HKA; Subkey: "Software\Classes\.au\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.au"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\au
Root: HKA; Subkey: "Software\Classes\.amr\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.amr"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\amr
Root: HKA; Subkey: "Software\Classes\.aob\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.aob"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\aob
Root: HKA; Subkey: "Software\Classes\.ape\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.ape"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\ape
Root: HKA; Subkey: "Software\Classes\.caf\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.caf"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\caf
Root: HKA; Subkey: "Software\Classes\.cda\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.cda"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\cda
Root: HKA; Subkey: "Software\Classes\.dts\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.dts"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\dts
Root: HKA; Subkey: "Software\Classes\.flac\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.flac"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\flac
Root: HKA; Subkey: "Software\Classes\.it\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.it"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\it
Root: HKA; Subkey: "Software\Classes\.m4a\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.m4a"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\m4a
Root: HKA; Subkey: "Software\Classes\.m4p\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.m4p"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\m4p
Root: HKA; Subkey: "Software\Classes\.mlp\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.mlp"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\mlp
Root: HKA; Subkey: "Software\Classes\.mod\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.mod"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\mod
Root: HKA; Subkey: "Software\Classes\.mp1\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.mp1"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\mp1
Root: HKA; Subkey: "Software\Classes\.mp2\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.mp2"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\mp2
Root: HKA; Subkey: "Software\Classes\.mp3\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.mp3"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\mp3
Root: HKA; Subkey: "Software\Classes\.mpc\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.mpc"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\mpc
Root: HKA; Subkey: "Software\Classes\.mpga\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.mpga"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\mpga
Root: HKA; Subkey: "Software\Classes\.oga\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.oga"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\oga
Root: HKA; Subkey: "Software\Classes\.oma\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.oma"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\oma
Root: HKA; Subkey: "Software\Classes\.opus\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.opus"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\opus
Root: HKA; Subkey: "Software\Classes\.qcp\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.qcp"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\qcp
Root: HKA; Subkey: "Software\Classes\.ra\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.ra"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\ra
Root: HKA; Subkey: "Software\Classes\.rmi\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.rmi"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\rmi
Root: HKA; Subkey: "Software\Classes\.snd\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.snd"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\snd
Root: HKA; Subkey: "Software\Classes\.s3m\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.s3m"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\s3m
Root: HKA; Subkey: "Software\Classes\.spx\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.spx"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\spx
Root: HKA; Subkey: "Software\Classes\.tta\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.tta"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\tta
Root: HKA; Subkey: "Software\Classes\.voc\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.voc"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\voc
Root: HKA; Subkey: "Software\Classes\.vqf\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.vqf"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\vqf
Root: HKA; Subkey: "Software\Classes\.w64\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.w64"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\w64
Root: HKA; Subkey: "Software\Classes\.wav\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.wav"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\wav
Root: HKA; Subkey: "Software\Classes\.wma\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.wma"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\wma
Root: HKA; Subkey: "Software\Classes\.wv\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.wv"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\wv
Root: HKA; Subkey: "Software\Classes\.xa\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.xa"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\xa
Root: HKA; Subkey: "Software\Classes\.xm\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.xm"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\audioFiles\xm

Root: HKA; Subkey: "Software\Classes\{#MyAppName}.3ga"; ValueType: string; ValueName: ""; ValueData: "3GA Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\3ga
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.669"; ValueType: string; ValueName: ""; ValueData: "669 Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\669
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.a52"; ValueType: string; ValueName: ""; ValueData: "A52 Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\a52
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.aac"; ValueType: string; ValueName: ""; ValueData: "AAC Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\aac
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ac3"; ValueType: string; ValueName: ""; ValueData: "AC3 Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\ac3
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.adt"; ValueType: string; ValueName: ""; ValueData: "ADT Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\adt
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.adts"; ValueType: string; ValueName: ""; ValueData: "ADTS Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\adts
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.aif"; ValueType: string; ValueName: ""; ValueData: "AIF Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\aif
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.aifc"; ValueType: string; ValueName: ""; ValueData: "AIFC Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\aifc
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.aiff"; ValueType: string; ValueName: ""; ValueData: "AIFF Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\aiff
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.au"; ValueType: string; ValueName: ""; ValueData: "AU Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\au
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.amr"; ValueType: string; ValueName: ""; ValueData: "AMR Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\amr
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.aob"; ValueType: string; ValueName: ""; ValueData: "AOB Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\aob
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ape"; ValueType: string; ValueName: ""; ValueData: "APE Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\ape
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.caf"; ValueType: string; ValueName: ""; ValueData: "CAF Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\caf
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.cda"; ValueType: string; ValueName: ""; ValueData: "CDA Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\cda
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.dts"; ValueType: string; ValueName: ""; ValueData: "DTS Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\dts
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.flac"; ValueType: string; ValueName: ""; ValueData: "FLAC Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\flac
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.it"; ValueType: string; ValueName: ""; ValueData: "IT Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\it
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m4a"; ValueType: string; ValueName: ""; ValueData: "M4A Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\m4a
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m4p"; ValueType: string; ValueName: ""; ValueData: "M4P Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\m4p
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mlp"; ValueType: string; ValueName: ""; ValueData: "MLP Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\mlp
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mod"; ValueType: string; ValueName: ""; ValueData: "MOD Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\mod
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mp1"; ValueType: string; ValueName: ""; ValueData: "MP1 Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\mp1
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mp2"; ValueType: string; ValueName: ""; ValueData: "MP2 Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\mp2
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mp3"; ValueType: string; ValueName: ""; ValueData: "MP3 Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\mp3
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpc"; ValueType: string; ValueName: ""; ValueData: "MPC Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\mpc
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpga"; ValueType: string; ValueName: ""; ValueData: "MPGA Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\mpga
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.oga"; ValueType: string; ValueName: ""; ValueData: "OGA Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\oga
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.oma"; ValueType: string; ValueName: ""; ValueData: "OMA Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\oma
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.opus"; ValueType: string; ValueName: ""; ValueData: "OPUS Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\opus
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.qcp"; ValueType: string; ValueName: ""; ValueData: "QCP Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\qcp
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ra"; ValueType: string; ValueName: ""; ValueData: "RA Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\ra
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.rmi"; ValueType: string; ValueName: ""; ValueData: "RMI Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\rmi
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.snd"; ValueType: string; ValueName: ""; ValueData: "SND Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\snd
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.s3m"; ValueType: string; ValueName: ""; ValueData: "S3M Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\s3m
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.spx"; ValueType: string; ValueName: ""; ValueData: "SPX Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\spx
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.tta"; ValueType: string; ValueName: ""; ValueData: "TTA Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\tta
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.voc"; ValueType: string; ValueName: ""; ValueData: "VOC Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\voc
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.vqf"; ValueType: string; ValueName: ""; ValueData: "VQF Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\vqf
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.w64"; ValueType: string; ValueName: ""; ValueData: "W64 Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\w64
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.wav"; ValueType: string; ValueName: ""; ValueData: "WAV Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\wav
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.wma"; ValueType: string; ValueName: ""; ValueData: "WMA Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\wma
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.wv"; ValueType: string; ValueName: ""; ValueData: "WV Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\wv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.xa"; ValueType: string; ValueName: ""; ValueData: "XA Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\xa
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.xm"; ValueType: string; ValueName: ""; ValueData: "XM Audio File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\audioFiles\xm

Root: HKA; Subkey: "Software\Classes\{#MyAppName}.3ga\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\3ga
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.669\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\669
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.a52\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\a52
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.aac\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\aac
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ac3\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\ac3
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.adt\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\adt
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.adts\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\adts
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.aif\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\aif
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.aifc\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\aifc
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.aiff\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\aiff
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.au\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\au
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.amr\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\amr
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.aob\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\aob
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ape\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\ape
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.caf\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\caf
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.cda\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\cda
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.dts\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\dts
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.flac\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\flac
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.it\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\it
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m4a\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\m4a
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m4p\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\m4p
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mlp\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\mlp
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mod\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\mod
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mp1\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\mp1
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mp2\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\mp2
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mp3\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\mp3
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpc\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\mpc
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpga\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\mpga
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.oga\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\oga
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.oma\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\oma
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.opus\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\opus
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.qcp\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\qcp
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ra\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\ra
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.rmi\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\rmi
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.snd\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\snd
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.s3m\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\s3m
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.spx\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\spx
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.tta\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\tta
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.voc\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\voc
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.vqf\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\vqf
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.w64\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\w64
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.wav\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\wav
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.wma\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\wma
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.wv\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\wv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.xa\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\xa
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.xm\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\audioFiles\xm

Root: HKA; Subkey: "Software\Classes\{#MyAppName}.3ga\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\3ga
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.669\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\669
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.a52\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\a52
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.aac\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\aac
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ac3\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\ac3
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.adt\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\adt
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.adts\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\adts
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.aif\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\aif
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.aifc\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\aifc
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.aiff\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\aiff
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.au\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\au
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.amr\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\amr
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.aob\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\aob
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ape\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\ape
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.caf\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\caf
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.cda\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\cda
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.dts\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\dts
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.flac\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\flac
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.it\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\it
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m4a\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\m4a
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m4p\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\m4p
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mlp\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\mlp
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mod\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\mod
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mp1\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\mp1
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mp2\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\mp2
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mp3\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\mp3
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpc\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\mpc
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpga\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\mpga
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.oga\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\oga
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.oma\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\oma
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.opus\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\opus
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.qcp\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\qcp
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ra\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\ra
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.rmi\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\rmi
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.snd\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\snd
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.s3m\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\s3m
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.spx\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\spx
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.tta\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\tta
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.voc\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\voc
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.vqf\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\vqf
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.w64\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\w64
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.wav\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\wav
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.wma\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\wma
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.wv\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\wv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.xa\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\xa
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.xm\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\audioFiles\xm



; Video File Associations
Root: HKA; Subkey: "Software\Classes\.3g2\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.3g2"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\3g2
Root: HKA; Subkey: "Software\Classes\.3gp\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.3gp"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\3gp
Root: HKA; Subkey: "Software\Classes\.3gp2\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.3gp2"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\3gp2
Root: HKA; Subkey: "Software\Classes\.3gpp\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.3gpp"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\3gpp
Root: HKA; Subkey: "Software\Classes\.amv\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.amv"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\amv
Root: HKA; Subkey: "Software\Classes\.asf\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.asf"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\asf
Root: HKA; Subkey: "Software\Classes\.avi\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.avi"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\avi
Root: HKA; Subkey: "Software\Classes\.bik\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.bik"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\bik
Root: HKA; Subkey: "Software\Classes\.dav\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.dav"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\dav
Root: HKA; Subkey: "Software\Classes\.divx\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.divx"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\divx
Root: HKA; Subkey: "Software\Classes\.drc\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.drc"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\drc
Root: HKA; Subkey: "Software\Classes\.dv\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.dv"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\dv
Root: HKA; Subkey: "Software\Classes\.dvr-ms\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.dvr-ms"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\dvrms
Root: HKA; Subkey: "Software\Classes\.evo\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.evo"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\evo
Root: HKA; Subkey: "Software\Classes\.f4v\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.f4v"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\f4v
Root: HKA; Subkey: "Software\Classes\.flv\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.flv"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\flv
Root: HKA; Subkey: "Software\Classes\.gvi\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.gvi"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\gvi
Root: HKA; Subkey: "Software\Classes\.gxf\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.gxf"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\gxf
Root: HKA; Subkey: "Software\Classes\.m1v\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.m1v"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\m1v
Root: HKA; Subkey: "Software\Classes\.m2t\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.m2t"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\m2t
Root: HKA; Subkey: "Software\Classes\.m2v\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.m2v"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\m2v
Root: HKA; Subkey: "Software\Classes\.m2ts\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.m2ts"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\m2ts
Root: HKA; Subkey: "Software\Classes\.m4v\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.m4v"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\m4v
Root: HKA; Subkey: "Software\Classes\.mkv\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.mkv"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\mkv
Root: HKA; Subkey: "Software\Classes\.mov\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.mov"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\mov
Root: HKA; Subkey: "Software\Classes\.mp2v\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.mp2v"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\mp2v
Root: HKA; Subkey: "Software\Classes\.mp4\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.mp4"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\mp4
Root: HKA; Subkey: "Software\Classes\.mp4v\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.mp4v"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\mp4v
Root: HKA; Subkey: "Software\Classes\.mpa\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.mpa"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\mpa
Root: HKA; Subkey: "Software\Classes\.mpe\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.mpe"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\mpe
Root: HKA; Subkey: "Software\Classes\.mpeg\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.mpeg"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\mpeg
Root: HKA; Subkey: "Software\Classes\.mpeg1\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.mpeg1"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\mpeg1
Root: HKA; Subkey: "Software\Classes\.mpeg2\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.mpeg2"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\mpeg2
Root: HKA; Subkey: "Software\Classes\.mpeg4\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.mpeg4"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\mpeg4
Root: HKA; Subkey: "Software\Classes\.mpg\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.mpg"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\mpg
Root: HKA; Subkey: "Software\Classes\.mpv2\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.mpv2"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\mpv2
Root: HKA; Subkey: "Software\Classes\.mts\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.mts"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\mts
Root: HKA; Subkey: "Software\Classes\.mtv\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.mtv"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\mtv
Root: HKA; Subkey: "Software\Classes\.mxf\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.mxf"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\mxf
Root: HKA; Subkey: "Software\Classes\.nsv\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.nsv"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\nsv
Root: HKA; Subkey: "Software\Classes\.nuv\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.nuv"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\nuv
Root: HKA; Subkey: "Software\Classes\.ogg\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.ogg"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\ogg
Root: HKA; Subkey: "Software\Classes\.ogm\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.ogm"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\ogm
Root: HKA; Subkey: "Software\Classes\.ogx\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.ogx"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\ogx
Root: HKA; Subkey: "Software\Classes\.ogv\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.ogv"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\ogv
Root: HKA; Subkey: "Software\Classes\.rec\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.rec"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\rec
Root: HKA; Subkey: "Software\Classes\.rm\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.rm"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\rm
Root: HKA; Subkey: "Software\Classes\.rmvb\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.rmvb"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\rmvb
Root: HKA; Subkey: "Software\Classes\.rpl\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.rpl"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\rpl
Root: HKA; Subkey: "Software\Classes\.thp\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.thp"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\thp
Root: HKA; Subkey: "Software\Classes\.tod\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.tod"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\tod
Root: HKA; Subkey: "Software\Classes\.tp\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.tp"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\tp
Root: HKA; Subkey: "Software\Classes\.ts\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.ts"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\ts
Root: HKA; Subkey: "Software\Classes\.tts\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.tts"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\tts
Root: HKA; Subkey: "Software\Classes\.vob\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.vob"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\vob
Root: HKA; Subkey: "Software\Classes\.vprj\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.vprj"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\vprj
Root: HKA; Subkey: "Software\Classes\.vro\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.vro"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\vro
Root: HKA; Subkey: "Software\Classes\.webm\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.webm"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\webm
Root: HKA; Subkey: "Software\Classes\.wmv\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.wmv"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\wmv
Root: HKA; Subkey: "Software\Classes\.wtv\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.wtv"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\wtv
Root: HKA; Subkey: "Software\Classes\.xesc\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.xesc"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\videoFiles\xesc

Root: HKA; Subkey: "Software\Classes\{#MyAppName}.3g2"; ValueType: string; ValueName: ""; ValueData: "3G2 Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\3g2
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.3gp"; ValueType: string; ValueName: ""; ValueData: "3GP Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\3gp
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.3gp2"; ValueType: string; ValueName: ""; ValueData: "3GP2 Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\3gp2
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.3gpp"; ValueType: string; ValueName: ""; ValueData: "3GPP Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\3gpp
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.amv"; ValueType: string; ValueName: ""; ValueData: "AMV Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\amv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.asf"; ValueType: string; ValueName: ""; ValueData: "ASF Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\asf
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.avi"; ValueType: string; ValueName: ""; ValueData: "AVI Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\avi
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.bik"; ValueType: string; ValueName: ""; ValueData: "BIK Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\bik
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.dav"; ValueType: string; ValueName: ""; ValueData: "DAV Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\dav
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.divx"; ValueType: string; ValueName: ""; ValueData: "DIVX Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\divx
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.drc"; ValueType: string; ValueName: ""; ValueData: "DRC Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\drc
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.dv"; ValueType: string; ValueName: ""; ValueData: "DV Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\dv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.dvr-ms"; ValueType: string; ValueName: ""; ValueData: "DVR-MS Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\dvrms
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.evo"; ValueType: string; ValueName: ""; ValueData: "EVO Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\evo
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.f4v"; ValueType: string; ValueName: ""; ValueData: "F4V Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\f4v
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.flv"; ValueType: string; ValueName: ""; ValueData: "FLV Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\flv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.gvi"; ValueType: string; ValueName: ""; ValueData: "GVI Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\gvi
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.gxf"; ValueType: string; ValueName: ""; ValueData: "GXF Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\gxf
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m1v"; ValueType: string; ValueName: ""; ValueData: "M1V Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\m1v
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m2t"; ValueType: string; ValueName: ""; ValueData: "M2T Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\m2t
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m2v"; ValueType: string; ValueName: ""; ValueData: "M2V Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\m2v
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m2ts"; ValueType: string; ValueName: ""; ValueData: "M2TS Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\m2ts
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m4v"; ValueType: string; ValueName: ""; ValueData: "M4V Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\m4v
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mkv"; ValueType: string; ValueName: ""; ValueData: "MKV Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\mkv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mov"; ValueType: string; ValueName: ""; ValueData: "MOV Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\mov
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mp2v"; ValueType: string; ValueName: ""; ValueData: "MP2V Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\mp2v
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mp4"; ValueType: string; ValueName: ""; ValueData: "MP4 Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\mp4
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mp4v"; ValueType: string; ValueName: ""; ValueData: "MP4V Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\mp4v
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpa"; ValueType: string; ValueName: ""; ValueData: "MPA Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\mpa
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpe"; ValueType: string; ValueName: ""; ValueData: "MPE Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\mpe
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpeg"; ValueType: string; ValueName: ""; ValueData: "MPEG Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\mpeg
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpeg1"; ValueType: string; ValueName: ""; ValueData: "MPEG1 Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\mpeg1
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpeg2"; ValueType: string; ValueName: ""; ValueData: "MPEG2 Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\mpeg2
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpeg4"; ValueType: string; ValueName: ""; ValueData: "MPEG4 Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\mpeg4
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpg"; ValueType: string; ValueName: ""; ValueData: "MPG Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\mpg
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpv2"; ValueType: string; ValueName: ""; ValueData: "MPV2 Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\mpv2
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mts"; ValueType: string; ValueName: ""; ValueData: "MTS Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\mts
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mtv"; ValueType: string; ValueName: ""; ValueData: "MTV Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\mtv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mxf"; ValueType: string; ValueName: ""; ValueData: "MXF Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\mxf
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.nsv"; ValueType: string; ValueName: ""; ValueData: "NSV Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\nsv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.nuv"; ValueType: string; ValueName: ""; ValueData: "NUV Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\nuv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ogg"; ValueType: string; ValueName: ""; ValueData: "OGG Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\ogg
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ogm"; ValueType: string; ValueName: ""; ValueData: "OGM Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\ogm
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ogx"; ValueType: string; ValueName: ""; ValueData: "OGX Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\ogx
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ogv"; ValueType: string; ValueName: ""; ValueData: "OGV Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\ogv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.rec"; ValueType: string; ValueName: ""; ValueData: "REC Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\rec
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.rm"; ValueType: string; ValueName: ""; ValueData: "RM Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\rm
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.rmvb"; ValueType: string; ValueName: ""; ValueData: "RMVB Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\rmvb
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.rpl"; ValueType: string; ValueName: ""; ValueData: "RPL Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\rpl
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.thp"; ValueType: string; ValueName: ""; ValueData: "THP Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\thp
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.tod"; ValueType: string; ValueName: ""; ValueData: "TOD Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\tod
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.tp"; ValueType: string; ValueName: ""; ValueData: "TP Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\tp
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ts"; ValueType: string; ValueName: ""; ValueData: "TS Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\ts
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.tts"; ValueType: string; ValueName: ""; ValueData: "TTS Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\tts
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.vob"; ValueType: string; ValueName: ""; ValueData: "VOB Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\vob
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.vprj"; ValueType: string; ValueName: ""; ValueData: "VPRJ Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\vprj
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.vro"; ValueType: string; ValueName: ""; ValueData: "VRO Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\vro
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.webm"; ValueType: string; ValueName: ""; ValueData: "WEBM Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\webm
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.wmv"; ValueType: string; ValueName: ""; ValueData: "WMV Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\wmv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.wtv"; ValueType: string; ValueName: ""; ValueData: "WTV Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\wtv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.xesc"; ValueType: string; ValueName: ""; ValueData: "XESC Video File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\videoFiles\xesc

Root: HKA; Subkey: "Software\Classes\{#MyAppName}.3g2\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\3g2
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.3gp\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\3gp
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.3gp2\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\3gp2
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.3gpp\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\3gpp
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.amv\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\amv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.asf\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\asf
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.avi\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\avi
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.bik\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\bik
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.dav\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\dav
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.divx\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\divx
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.drc\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\drc
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.dv\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\dv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.dvr-ms\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\dvrms
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.evo\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\evo
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.f4v\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\f4v
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.flv\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\flv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.gvi\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\gvi
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.gxf\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\gxf
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m1v\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\m1v
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m2t\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\m2t
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m2v\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\m2v
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m2ts\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\m2ts
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m4v\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\m4v
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mkv\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\mkv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mov\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\mov
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mp2v\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\mp2v
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mp4\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\mp4
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mp4v\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\mp4v
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpa\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\mpa
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpe\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\mpe
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpeg\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\mpeg
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpeg1\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\mpeg1
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpeg2\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\mpeg2
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpeg4\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\mpeg4
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpg\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\mpg
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpv2\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\mpv2
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mts\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\mts
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mtv\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\mtv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mxf\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\mxf
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.nsv\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\nsv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.nuv\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\nuv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ogg\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\ogg
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ogm\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\ogm
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ogx\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\ogx
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ogv\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\ogv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.rec\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\rec
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.rm\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\rm
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.rmvb\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\rmvb
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.rpl\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\rpl
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.thp\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\thp
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.tod\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\tod
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.tp\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\tp
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ts\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\ts
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.tts\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\tts
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.vob\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\vob
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.vprj\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\vprj
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.vro\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\vro
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.webm\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\webm
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.wmv\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\wmv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.wtv\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\wtv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.xesc\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\videoFiles\xesc

Root: HKA; Subkey: "Software\Classes\{#MyAppName}.3g2\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\3g2
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.3gp\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\3gp
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.3gp2\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\3gp2
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.3gpp\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\3gpp
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.amv\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\amv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.asf\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\asf
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.avi\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\avi
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.bik\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\bik
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.dav\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\dav
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.divx\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\divx
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.drc\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\drc
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.dv\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\dv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.dvr-ms\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\dvrms
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.evo\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\evo
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.f4v\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\f4v
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.flv\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\flv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.gvi\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\gvi
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.gxf\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\gxf
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m1v\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\m1v
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m2t\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\m2t
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m2v\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\m2v
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m2ts\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\m2ts
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m4v\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\m4v
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mkv\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\mkv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mov\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\mov
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mp2v\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\mp2v
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mp4\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\mp4
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mp4v\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\mp4v
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpa\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\mpa
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpe\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\mpe
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpeg\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\mpeg
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpeg1\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\mpeg1
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpeg2\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\mpeg2
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpeg4\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\mpeg4
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpg\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\mpg
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mpv2\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\mpv2
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mts\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\mts
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mtv\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\mtv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.mxf\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\mxf
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.nsv\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\nsv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.nuv\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\nuv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ogg\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\ogg
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ogm\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\ogm
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ogx\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\ogx
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ogv\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\ogv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.rec\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\rec
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.rm\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\rm
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.rmvb\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\rmvb
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.rpl\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\rpl
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.thp\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\thp
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.tod\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\tod
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.tp\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\tp
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.ts\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\ts
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.tts\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\tts
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.vob\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\vob
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.vprj\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\vprj
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.vro\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\vro
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.webm\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\webm
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.wmv\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\wmv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.wtv\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\wtv
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.xesc\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\videoFiles\xesc



; Other File Associations
Root: HKA; Subkey: "Software\Classes\.b4s\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.b4s"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\otherFiles\b4s
Root: HKA; Subkey: "Software\Classes\.hkey\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.hkey"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\otherFiles\hkey
Root: HKA; Subkey: "Software\Classes\.hmrk\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.hmrk"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\otherFiles\hmrk
Root: HKA; Subkey: "Software\Classes\.hpl\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.hpl"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\otherFiles\hpl
Root: HKA; Subkey: "Software\Classes\.hprj\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.hprj"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\otherFiles\hprj
Root: HKA; Subkey: "Software\Classes\.m3u\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.m3u"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\otherFiles\m3u
Root: HKA; Subkey: "Software\Classes\.m3u8\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.m3u8"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\otherFiles\m3u8
Root: HKA; Subkey: "Software\Classes\.pls\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.pls"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\otherFiles\pls
Root: HKA; Subkey: "Software\Classes\.wpl\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.wpl"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\otherFiles\wpl
Root: HKA; Subkey: "Software\Classes\.vpl\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.vpl"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\otherFiles\vpl
Root: HKA; Subkey: "Software\Classes\.xspf\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}.xspf"; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\otherFiles\xspf
; Root: HKA; Subkey: "Software\Classes\.\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppName}."; ValueData: ""; Flags: uninsdeletevalue; Components: fileTypeAssociations\otherFiles\

Root: HKA; Subkey: "Software\Classes\{#MyAppName}.b4s"; ValueType: string; ValueName: ""; ValueData: "B4S Other File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\otherFiles\b4s
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.hkey"; ValueType: string; ValueName: ""; ValueData: "HPlayer Hotkey File"; Flags: uninsdeletekey; Components: fileTypeAssociations\otherFiles\hkey
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.hmrk"; ValueType: string; ValueName: ""; ValueData: "HPlayer Marker File"; Flags: uninsdeletekey; Components: fileTypeAssociations\otherFiles\hmrk
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.hpl"; ValueType: string; ValueName: ""; ValueData: "HPlayer Playlist File"; Flags: uninsdeletekey; Components: fileTypeAssociations\otherFiles\hpl
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.hprj"; ValueType: string; ValueName: ""; ValueData: "HPlayer Project File"; Flags: uninsdeletekey; Components: fileTypeAssociations\otherFiles\hprj
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m3u"; ValueType: string; ValueName: ""; ValueData: "M3U Other File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\otherFiles\m3u
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m3u8"; ValueType: string; ValueName: ""; ValueData: "M3U Audio Playlist File (UTF-8)"; Flags: uninsdeletekey; Components: fileTypeAssociations\otherFiles\m3u8
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.pls"; ValueType: string; ValueName: ""; ValueData: "PLS Other File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\otherFiles\pls
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.wpl"; ValueType: string; ValueName: ""; ValueData: "WPL Other File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\otherFiles\wpl
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.vpl"; ValueType: string; ValueName: ""; ValueData: "{#MyAppName} Playlist File"; Flags: uninsdeletekey; Components: fileTypeAssociations\otherFiles\vpl
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.xspf"; ValueType: string; ValueName: ""; ValueData: "XSPF Other File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\otherFiles\xspf
; Root: HKA; Subkey: "Software\Classes\{#MyAppName}."; ValueType: string; ValueName: ""; ValueData: "{#MyAppName} File ({#MyAppName})"; Flags: uninsdeletekey; Components: fileTypeAssociations\otherFiles\

Root: HKA; Subkey: "Software\Classes\{#MyAppName}.b4s\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\otherFiles\b4s
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.hkey\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\otherFiles\hkey
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.hmrk\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\otherFiles\hmrk
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.hpl\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\otherFiles\hpl
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.hprj\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\otherFiles\hprj
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m3u\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\otherFiles\m3u
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m3u8\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\otherFiles\m3u8
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.pls\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\otherFiles\pls
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.wpl\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\otherFiles\wpl
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.vpl\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\otherFiles\vpl
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.xspf\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\otherFiles\xspf
; Root: HKA; Subkey: "Software\Classes\{#MyAppName}.\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Components: fileTypeAssociations\otherFiles\

Root: HKA; Subkey: "Software\Classes\{#MyAppName}.b4s\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\otherFiles\b4s
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.hkey\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\otherFiles\hkey
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.hmrk\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\otherFiles\hmrk
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.hpl\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\otherFiles\hpl
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.hprj\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\otherFiles\hprj
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m3u\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\otherFiles\m3u
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.m3u8\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\otherFiles\m3u8
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.pls\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\otherFiles\pls
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.wpl\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\otherFiles\wpl
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.vpl\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\otherFiles\vpl
Root: HKA; Subkey: "Software\Classes\{#MyAppName}.xspf\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\otherFiles\xspf
; Root: HKA; Subkey: "Software\Classes\{#MyAppName}.\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Components: fileTypeAssociations\otherFiles\



[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Components: desktopShortcut
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"; Components: startMenuShortcut
Name: "{group}\Uninstall {#MyAppName}"; Filename: "{uninstallexe}"; Components: startMenuShortcut


[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
