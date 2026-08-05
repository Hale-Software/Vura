# ============================================================================
# NSIS Installer Script for Custom Qt/FFmpeg Media Player
# ============================================================================

# Handle setting application version being passed from command line or default to 0.0.0-dev
!ifndef VERSION
  !define VERSION "0.0.0-dev"
!endif

!define APP_NAME "Vura"
!define COMP_NAME "Hale Software"
!define EXE_NAME "vura.exe"
!define UNINSTALL_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"

# Define installer name and compression type
Name "${APP_NAME}"
OutFile "vura-${VERSION}-windows.exe"
InstallDir "$PROGRAMFILES64\${APP_NAME}"
SetCompressor /SOLID lzma

# Request administrative execution level for Windows privileges
RequestExecutionLevel admin

# Variables
Var StartMenuFolder

# Include modern user interface design elements
!include "MUI2.nsh"

# Interface Settings
!define MUI_ABORTWARNING
!define MUI_LANGDLL_ALLLANGUAGES
!define MUI_ICON "vura.ico"
!define MUI_UNICON "vura.ico"

# Remember the installed language for future uninstalls
!define MUI_LANGDLL_REGISTRY_ROOT "HKCU"
!define MUI_LANGDLL_REGISTRY_KEY "Software\Vura"
!define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

# Page layout definitions
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "license.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY

# Start Menu Folder Page Configuration
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Vura"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder

!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_RUN "$INSTDIR\${EXE_NAME}"
!define MUI_FINISHPAGE_RUN_TEXT "Launch Vura"

!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

# Languages
!insertmacro MUI_LANGUAGE "English" # Default language
!insertmacro MUI_LANGUAGE "Spanish"
!insertmacro MUI_LANGUAGE "German"

# ============================================================================
# FILE ASSOCIATION HANDLERS
# ============================================================================

# Application File Association Program IDs (ProgIDs) for Windows
!define PROG_ID_VHK "CustomMediaPlayer.AssocFile.VHK"
!define PROG_ID_VPL "CustomMediaPlayer.AssocFile.VPL"
!define PROG_ID_VPRJ "CustomMediaPlayer.AssocFile.VPRJ"
!define PROG_ID_VVM "CustomMediaPlayer.AssocFile.VVM"

# Audio File Associations Program IDs (ProgIDs) for Windows
!define PROG_ID_3GA "CustomMediaPlayer.AssocFile.3GA"
!define PROG_ID_669 "CustomMediaPlayer.AssocFile.669"
!define PROG_ID_A52 "CustomMediaPlayer.AssocFile.A52"
!define PROG_ID_AAC "CustomMediaPlayer.AssocFile.AAC"
!define PROG_ID_AC3 "CustomMediaPlayer.AssocFile.AC3"
!define PROG_ID_ADT "CustomMediaPlayer.AssocFile.ADT"
!define PROG_ID_ADTS "CustomMediaPlayer.AssocFile.ADTS"
!define PROG_ID_AIF "CustomMediaPlayer.AssocFile.AIF"
!define PROG_ID_AIFC "CustomMediaPlayer.AssocFile.AIFC"
!define PROG_ID_AIFF "CustomMediaPlayer.AssocFile.AIFF"
!define PROG_ID_AU "CustomMediaPlayer.AssocFile.AU"
!define PROG_ID_AMR "CustomMediaPlayer.AssocFile.AMR"
!define PROG_ID_AOB "CustomMediaPlayer.AssocFile.AOB"
!define PROG_ID_APE "CustomMediaPlayer.AssocFile.APE"
!define PROG_ID_CAF "CustomMediaPlayer.AssocFile.CAF"
!define PROG_ID_CDA "CustomMediaPlayer.AssocFile.CDA"
!define PROG_ID_DTS "CustomMediaPlayer.AssocFile.DTS"
!define PROG_ID_FLAC "CustomMediaPlayer.AssocFile.FLAC"
!define PROG_ID_IT "CustomMediaPlayer.AssocFile.IT"
!define PROG_ID_M4A "CustomMediaPlayer.AssocFile.M4A"
!define PROG_ID_M4P "CustomMediaPlayer.AssocFile.M4P"
!define PROG_ID_MLP "CustomMediaPlayer.AssocFile.MLP"
!define PROG_ID_MOD "CustomMediaPlayer.AssocFile.MOD"
!define PROG_ID_MP1 "CustomMediaPlayer.AssocFile.MP1"
!define PROG_ID_MP2 "CustomMediaPlayer.AssocFile.MP2"
!define PROG_ID_MP3 "CustomMediaPlayer.AssocFile.MP3"
!define PROG_ID_MPC "CustomMediaPlayer.AssocFile.MPC"
!define PROG_ID_MPGA "CustomMediaPlayer.AssocFile.MPGA"
!define PROG_ID_OGA "CustomMediaPlayer.AssocFile.OGA"
!define PROG_ID_OMA "CustomMediaPlayer.AssocFile.OMA"
!define PROG_ID_OPUS "CustomMediaPlayer.AssocFile.OPUS"
!define PROG_ID_QCP "CustomMediaPlayer.AssocFile.QCP"
!define PROG_ID_RA "CustomMediaPlayer.AssocFile.RA"
!define PROG_ID_RMI "CustomMediaPlayer.AssocFile.RMI"
!define PROG_ID_SND "CustomMediaPlayer.AssocFile.SND"
!define PROG_ID_S3M "CustomMediaPlayer.AssocFile.S3M"
!define PROG_ID_SPX "CustomMediaPlayer.AssocFile.SPX"
!define PROG_ID_TTA "CustomMediaPlayer.AssocFile.TTA"
!define PROG_ID_VOC "CustomMediaPlayer.AssocFile.VOC"
!define PROG_ID_VQF "CustomMediaPlayer.AssocFile.VQF"
!define PROG_ID_W64 "CustomMediaPlayer.AssocFile.W64"
!define PROG_ID_WAV "CustomMediaPlayer.AssocFile.WAV"
!define PROG_ID_WMA "CustomMediaPlayer.AssocFile.WMA"
!define PROG_ID_WV "CustomMediaPlayer.AssocFile.WV"
!define PROG_ID_XA "CustomMediaPlayer.AssocFile.XA"
!define PROG_ID_XM "CustomMediaPlayer.AssocFile.XM"

# Video File Associations Program IDs (ProgIDs) for Windows
!define PROG_ID_3G2 "CustomMediaPlayer.AssocFile.3G2"
!define PROG_ID_3GP "CustomMediaPlayer.AssocFile.3GP"
!define PROG_ID_3GP2 "CustomMediaPlayer.AssocFile.3GP2"
!define PROG_ID_3GPP "CustomMediaPlayer.AssocFile.3GPP"
!define PROG_ID_AMV "CustomMediaPlayer.AssocFile.AMV"
!define PROG_ID_ASF "CustomMediaPlayer.AssocFile.ASF"
!define PROG_ID_AVI "CustomMediaPlayer.AssocFile.AVI"
!define PROG_ID_BIK "CustomMediaPlayer.AssocFile.BIK"
!define PROG_ID_DAV "CustomMediaPlayer.AssocFile.DAV"
!define PROG_ID_DIVX "CustomMediaPlayer.AssocFile.DIVX"
!define PROG_ID_DRC "CustomMediaPlayer.AssocFile.DRC"
!define PROG_ID_DV "CustomMediaPlayer.AssocFile.DV"

!define PROG_ID_M4V "CustomMediaPlayer.AssocFile.M4V"
!define PROG_ID_MKV "CustomMediaPlayer.AssocFile.MKV"
!define PROG_ID_MOV "CustomMediaPlayer.AssocFile.MOV"
!define PROG_ID_MP2V "CustomMediaPlayer.AssocFile.MP2V"
!define PROG_ID_MP4 "CustomMediaPlayer.AssocFile.MP4"
!define PROG_ID_MP4V "CustomMediaPlayer.AssocFile.MP4V"
!define PROG_ID_MPEG "CustomMediaPlayer.AssocFile.MPEG"
!define PROG_ID_MPEG1 "CustomMediaPlayer.AssocFile.MPEG1"
!define PROG_ID_MPEG2 "CustomMediaPlayer.AssocFile.MPEG2"
!define PROG_ID_MPEG4 "CustomMediaPlayer.AssocFile.MPEG4"
!define PROG_ID_MPG "CustomMediaPlayer.AssocFile.MPG"
!define PROG_ID_OGG "CustomMediaPlayer.AssocFile.OGG"
!define PROG_ID_WEBM "CustomMediaPlayer.AssocFile.WEBM"
!define PROG_ID_WMV "CustomMediaPlayer.AssocFile.WMV"

# Other File Associations Program IDs (ProgIDs) for Windows
!define PROG_ID_B4S "CustomMediaPlayer.AssocFile.B4S"
!define PROG_ID_M3U "CustomMediaPlayer.AssocFile.M3U"
!define PROG_ID_M3U8 "CustomMediaPlayer.AssocFile.M3U8"
!define PROG_ID_PLS "CustomMediaPlayer.AssocFile.PLS"
!define PROG_ID_WPL "CustomMediaPlayer.AssocFile.WPL"
!define PROG_ID_XSPF "CustomMediaPlayer.AssocFile.XSPF"

# ============================================================================
# INSTALLATION SECTION
# ============================================================================
Section "Media Player (Required)" SecCore
    SectionIn RO

    SetOutPath "$INSTDIR"

    # Store Installation Path in Registry for future reference
    WriteRegStr HKCU "Software\Vura" "" $INSTDIR

    # Main Executable and Core Plugins
    File "dist_windows\${EXE_NAME}"
    File "dist_windows\*.dll"
    File /r "dist_windows\assets"

    # Copy Qt Runtime Directory Trees
    File /r "dist_windows\generic"
    File /r "dist_windows\iconengines"
    File /r "dist_windows\imageformats"
    File /r "dist_windows\multimedia"
    File /r "dist_windows\platforms"
    File /r "dist_windows\styles"
    File /r "dist_windows\tls"
    File /r "dist_windows\networkinformation"
    File /r "dist_windows\translations"

    # Write Registry Uninstall Entries for Windows Control Panel
    WriteUninstaller "$INSTDIR\uninstall.exe"
    WriteRegStr HKLM "${UNINSTALL_KEY}" "DisplayName" "${APP_NAME}"
    WriteRegStr HKLM "${UNINSTALL_KEY}" "UninstallString" '"$INSTDIR\uninstall.exe"'
    WriteRegStr HKLM "${UNINSTALL_KEY}" "DisplayVersion" "${VERSION}"
    WriteRegStr HKLM "${UNINSTALL_KEY}" "Publisher" "${COMP_NAME}"
    WriteRegDWORD HKLM "${UNINSTALL_KEY}" "NoModify" 1
    WriteRegDWORD HKLM "${UNINSTALL_KEY}" "NoRepair" 1

    # Register Application Capabilities (Windows 10/11 Preferred Method)
    WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities" "ApplicationName" "${APP_NAME}"
    WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities" "ApplicationDescription" "Free and open source video player and editor designed for customization."

    # Write Registry Entries for Custom URI Scheme
    WriteRegStr HKLM "Software\Classes\${APP_NAME}" "" "URL:Vura Protocol"
    WriteRegStr HKLM "Software\Classes\${APP_NAME}" "URL Protocol" ""
    WriteRegStr HKLM "Software\Classes\${APP_NAME}\DefaultIcon" "" "$INSTDIR\${EXE_NAME},0"
    WriteRegStr HKLM "Software\Classes\${APP_NAME}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1" "--network"'


SectionEnd

# Optional Section for Desktop Shortcut
Section "Create Desktop Shortcut" SecDesktopShortcut
    CreateShortcut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${EXE_NAME}"
SectionEnd

# Optional Section for Start Menu Shortcut
Section "Create Start Menu Shortcut" SecStartMenuShortcut
    CreateDirectory "$SMPROGRAMS\${APP_NAME}"
    CreateShortcut "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk" "$INSTDIR\${EXE_NAME}"
    CreateShortcut "$SMPROGRAMS\${APP_NAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

# Optional Section for File Associations
SectionGroup "File Associations" SecFileAssociations

    # Application File Associations
    SectionGroup "Application Files" ApplicationFilesGroup

        Section ".vhk"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_VHK}"
            WriteRegStr HKLM "Software\Classes\.vhk\OpenWithProgids" "${PROG_ID_VHK}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_VHK}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_VHK}" "" "Vura Hotkey File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_VHK}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".vpl"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_VPL}"
            WriteRegStr HKLM "Software\Classes\.vpl\OpenWithProgids" "${PROG_ID_VPL}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_VPL}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_VPL}" "" "Vura Playlist File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_VPL}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".vprj"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_VPRJ}"
            WriteRegStr HKLM "Software\Classes\.vprj\OpenWithProgids" "${PROG_ID_VPRJ}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_VPRJ}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_VPRJ}" "" "Vura Project File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_VPRJ}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".vvm"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_VVM}"
            WriteRegStr HKLM "Software\Classes\.vvm\OpenWithProgids" "${PROG_ID_VVM}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_VVM}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_VVM}" "" "Vura Marker File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_VVM}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

    SectionGroupEnd


    # Audio File Associations
    SectionGroup "Audio Files" AudioFilesGroup

        Section ".3ga"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_3GA}"
            WriteRegStr HKLM "Software\Classes\.3ga\OpenWithProgids" "${PROG_ID_3GA}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_3GA}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_3GA}" "" "3GA Audio File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_3GA}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".a52"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_A52}"
            WriteRegStr HKLM "Software\Classes\.a52\OpenWithProgids" "${PROG_ID_A52}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_A52}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_A52}" "" "A52 Audio File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_A52}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".aac"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_AAC}"
            WriteRegStr HKLM "Software\Classes\.aac\OpenWithProgids" "${PROG_ID_AAC}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_AAC}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_AAC}" "" "AAC Audio File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_AAC}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".m4a"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_M4A}"
            WriteRegStr HKLM "Software\Classes\.m4a\OpenWithProgids" "${PROG_ID_M4A}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_M4A}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_M4A}" "" "M4A Audio File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_M4A}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".m4p"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_M4P}"
            WriteRegStr HKLM "Software\Classes\.m4p\OpenWithProgids" "${PROG_ID_M4P}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_M4P}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_M4P}" "" "M4P Audio File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_M4P}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".mp1"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_MP1}"
            WriteRegStr HKLM "Software\Classes\.mp1\OpenWithProgids" "${PROG_ID_MP1}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MP1}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MP1}" "" "MP1 Audio File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MP1}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".mp2"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_MP2}"
            WriteRegStr HKLM "Software\Classes\.mp2\OpenWithProgids" "${PROG_ID_MP2}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MP2}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MP2}" "" "MP2 Audio File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MP2}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".mp3"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_MP3}"
            WriteRegStr HKLM "Software\Classes\.mp3\OpenWithProgids" "${PROG_ID_MP3}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MP3}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MP3}" "" "MP3 Audio File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MP3}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".mpga"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_MPGA}"
            WriteRegStr HKLM "Software\Classes\.mpga\OpenWithProgids" "${PROG_ID_MPGA}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MPGA}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MPGA}" "" "MPGA Audio File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MPGA}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".voc"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_VOC}"
            WriteRegStr HKLM "Software\Classes\.voc\OpenWithProgids" "${PROG_ID_VOC}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_VOC}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_VOC}" "" "VOC Audio File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_VOC}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".wav"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_WAV}"
            WriteRegStr HKLM "Software\Classes\.wav\OpenWithProgids" "${PROG_ID_WAV}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_WAV}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_WAV}" "" "WAV Audio File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_WAV}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".wma"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_WMA}"
            WriteRegStr HKLM "Software\Classes\.wma\OpenWithProgids" "${PROG_ID_WMA}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_WMA}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_WMA}" "" "WMA Audio File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_WMA}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

    SectionGroupEnd


    # Video File Associations
    SectionGroup "Video Files" VideoFilesGroup

        Section ".avi"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_AVI}"
            WriteRegStr HKLM "Software\Classes\.avi\OpenWithProgids" "${PROG_ID_AVI}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_AVI}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_AVI}" "" "AVI Video File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_AVI}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".bik"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_BIK}"
            WriteRegStr HKLM "Software\Classes\.bik\OpenWithProgids" "${PROG_ID_BIK}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_BIK}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_BIK}" "" "BIK Video File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_BIK}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".mkv"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_MKV}"
            WriteRegStr HKLM "Software\Classes\.mkv\OpenWithProgids" "${PROG_ID_MKV}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MKV}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MKV}" "" "MKV Video File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MKV}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".mov"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_MOV}"
            WriteRegStr HKLM "Software\Classes\.mov\OpenWithProgids" "${PROG_ID_MOV}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MOV}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MOV}" "" "MOV Video File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MOV}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".mp4"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_MP4}"
            WriteRegStr HKLM "Software\Classes\.mp4\OpenWithProgids" "${PROG_ID_MP4}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MP4}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MP4}" "" "MP4 Video File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MP4}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".mp4v"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_MP4V}"
            WriteRegStr HKLM "Software\Classes\.mp4v\OpenWithProgids" "${PROG_ID_MP4V}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MP4V}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MP4V}" "" "MP4V Video File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MP4V}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".mpeg"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_MPEG}"
            WriteRegStr HKLM "Software\Classes\.mpeg\OpenWithProgids" "${PROG_ID_MPEG}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MPEG}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MPEG}" "" "MPEG Video File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MPEG}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".mpeg1"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_MPEG1}"
            WriteRegStr HKLM "Software\Classes\.mpeg1\OpenWithProgids" "${PROG_ID_MPEG1}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MPEG1}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MPEG1}" "" "MPEG1 Video File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MPEG1}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".mpeg2"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_MPEG2}"
            WriteRegStr HKLM "Software\Classes\.mpeg2\OpenWithProgids" "${PROG_ID_MPEG2}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MPEG2}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MPEG2}" "" "MPEG2 Video File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MPEG2}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".mpeg4"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_MPEG4}"
            WriteRegStr HKLM "Software\Classes\.mpeg4\OpenWithProgids" "${PROG_ID_MPEG4}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MPEG4}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MPEG4}" "" "MPEG4 Video File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MPEG4}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".mpg"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_MPG}"
            WriteRegStr HKLM "Software\Classes\.mpg\OpenWithProgids" "${PROG_ID_MPG}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MPG}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MPG}" "" "MPG Video File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_MPG}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".ogg"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_OGG}"
            WriteRegStr HKLM "Software\Classes\.ogg\OpenWithProgids" "${PROG_ID_OGG}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_OGG}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_OGG}" "" "OGG Video File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_OGG}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".webm"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_WEBM}"
            WriteRegStr HKLM "Software\Classes\.webm\OpenWithProgids" "${PROG_ID_WEBM}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_WEBM}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_WEBM}" "" "WebM Video File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_WEBM}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".wmv"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_WMV}"
            WriteRegStr HKLM "Software\Classes\.wmv\OpenWithProgids" "${PROG_ID_WMV}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_WMV}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_WMV}" "" "WMV Video File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_WMV}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

    SectionGroupEnd


    # Misc File Associations
    SectionGroup "Misc Files" MiscFilesGroup

        Section ".b4s"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_B4S}"
            WriteRegStr HKLM "Software\Classes\.b4s\OpenWithProgids" "${PROG_ID_B4S}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_B4S}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_B4S}" "" "Vura Playlist File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_B4S}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".m3u"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_M3U}"
            WriteRegStr HKLM "Software\Classes\.m3u\OpenWithProgids" "${PROG_ID_M3U}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_M3U}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_M3U}" "" "Vura Playlist File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_M3U}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".m3u8"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_M3U8}"
            WriteRegStr HKLM "Software\Classes\.m3u8\OpenWithProgids" "${PROG_ID_M3U8}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_M3U8}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_M3U8}" "" "Vura Playlist File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_M3U8}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".pls"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_PLS}"
            WriteRegStr HKLM "Software\Classes\.pls\OpenWithProgids" "${PROG_ID_PLS}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_PLS}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_PLS}" "" "Vura Playlist File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_PLS}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".wpl"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_WPL}"
            WriteRegStr HKLM "Software\Classes\.wpl\OpenWithProgids" "${PROG_ID_WPL}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_WPL}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_WPL}" "" "Vura Playlist File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_WPL}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

        Section ".xspf"
            WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "." "${PROG_ID_XSPF}"
            WriteRegStr HKLM "Software\Classes\.xspf\OpenWithProgids" "${PROG_ID_XSPF}" ""
            WriteRegStr HKLM "Software\Classes\${PROG_ID_XSPF}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
            WriteRegStr HKLM "Software\Classes\${PROG_ID_XSPF}" "" "Vura Playlist File"
            WriteRegStr HKLM "Software\Classes\${PROG_ID_XSPF}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
        SectionEnd

    SectionGroupEnd

SectionGroupEnd

# Hidden section (name starts with "-" so it's never shown/unchecked in the
# component list) that always runs after install to finalize file association
# registration, regardless of which individual associations were selected above.
Section "-FileAssocFinalize"
    # Register with RegisteredApplications so Windows settings app recognizes it
    WriteRegStr HKLM "Software\RegisteredApplications" "${APP_NAME}" "Software\Clients\Media\${APP_NAME}\Capabilities"

    # Notify the Windows Shell API to refresh icons and default associations immediately
    System::Call 'shell32::SHChangeNotify(i 0x08000000, i 0, i 0, i 0)'
SectionEnd

# Optional Section for Context Menu
Section "Context Menus" SecContextMenus

	# Add Windows Registry Entries for Open File Context Menus
	WriteRegStr HKLM "Software\Classes\*\shell\${APP_NAME}" "" "Play with ${APP_NAME}"
	WriteRegStr HKLM "Software\Classes\*\shell\${APP_NAME}" "Icon" "$INSTDIR\${EXE_NAME},0"
	WriteRegStr HKLM "Software\Classes\*\shell\${APP_NAME}\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
	
	# Add Windows Registry Entries for Open Directory Context Menus
	WriteRegStr HKLM "Software\Classes\Directory\shell\${APP_NAME}" "" "Open Folder in ${APP_NAME}"
	WriteRegStr HKLM "Software\Classes\Directory\shell\${APP_NAME}"  "Icon" "$INSTDIR\${EXE_NAME},0"
	WriteRegStr HKLM "Software\Classes\Directory\shell\${APP_NAME}\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
	
	WriteRegStr HKLM "Software\Classes\Directory\Background\shell\${APP_NAME}" "" "Open Folder in ${APP_NAME}"
	WriteRegStr HKLM "Software\Classes\Directory\Background\shell\${APP_NAME}"  "Icon" "$INSTDIR\${EXE_NAME},0"
	WriteRegStr HKLM "Software\Classes\Directory\Background\shell\${APP_NAME}\command" "" '"$INSTDIR\${EXE_NAME}" "%V"'
	
SectionEnd


# ============================================================================
# UNINSTALLATION SECTION
# ============================================================================
Section "Uninstall"
    # Kill any active media player instance to avoid file lock blocking errors
    ExecWait 'taskkill /F /IM ${EXE_NAME}'

    # Delete local files and plugins completely
    Delete "$INSTDIR\${EXE_NAME}"
    Delete "$INSTDIR\*.dll"
    Delete "$INSTDIR\uninstall.exe"
    RMDir /r "$INSTDIR\assets"
    RMDir /r "$INSTDIR\generic"
    RMDir /r "$INSTDIR\iconengines"
    RMDir /r "$INSTDIR\imageformats"
    RMDir /r "$INSTDIR\multimedia"
    RMDir /r "$INSTDIR\platforms"
    RMDir /r "$INSTDIR\styles"
    RMDir /r "$INSTDIR\tls"
    RMDir /r "$INSTDIR\networkinformation"
    RMDir /r "$INSTDIR\translations"
    RMDir "$INSTDIR"

    # Wipe system shortcuts
    Delete "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk"
    Delete "$SMPROGRAMS\${APP_NAME}\Uninstall.lnk"
    RMDir "$SMPROGRAMS\${APP_NAME}"
    Delete "$DESKTOP\${APP_NAME}.lnk"

    # Remove Windows Add/Remove configuration registry branches
    DeleteRegKey HKLM "${UNINSTALL_KEY}"

    # Wipe programmatic handler blocks
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_VHK}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_VPL}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_VPRJ}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_VVM}"

    DeleteRegKey HKLM "Software\Classes\${PROG_ID_3GA}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_A52}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_AAC}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_M4A}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_M4P}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_MP1}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_MP2}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_MP3}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_MPGA}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_VOC}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_WAV}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_WMA}"

    DeleteRegKey HKLM "Software\Classes\${PROG_ID_AVI}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_BIK}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_MKV}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_MOV}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_MP4}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_MP4V}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_MPEG}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_MPEG1}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_MPEG2}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_MPEG4}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_MPG}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_OGG}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_WEBM}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_WMV}"

    DeleteRegKey HKLM "Software\Classes\${PROG_ID_B4S}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_M3U}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_M3U8}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_PLS}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_WPL}"
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_XSPF}"

    # Remove app entry mapping inside OpenWith lists
    DeleteRegValue HKLM "Software\Classes\.vhk\OpenWithProgids" "${PROG_ID_VHK}"
    DeleteRegValue HKLM "Software\Classes\.vpl\OpenWithProgids" "${PROG_ID_VPL}"
    DeleteRegValue HKLM "Software\Classes\.vprj\OpenWithProgids" "${PROG_ID_VPRJ}"
    DeleteRegValue HKLM "Software\Classes\.vvm\OpenWithProgids" "${PROG_ID_VVM}"

    DeleteRegValue HKLM "Software\Classes\.3ga\OpenWithProgids" "${PROG_ID_3GA}"
    DeleteRegValue HKLM "Software\Classes\.a52\OpenWithProgids" "${PROG_ID_A52}"
    DeleteRegValue HKLM "Software\Classes\.aac\OpenWithProgids" "${PROG_ID_AAC}"
    DeleteRegValue HKLM "Software\Classes\.m4a\OpenWithProgids" "${PROG_ID_M4A}"
    DeleteRegValue HKLM "Software\Classes\.m4p\OpenWithProgids" "${PROG_ID_M4P}"
    DeleteRegValue HKLM "Software\Classes\.mp1\OpenWithProgids" "${PROG_ID_MP1}"
    DeleteRegValue HKLM "Software\Classes\.mp2\OpenWithProgids" "${PROG_ID_MP2}"
    DeleteRegValue HKLM "Software\Classes\.mp3\OpenWithProgids" "${PROG_ID_MP3}"
    DeleteRegValue HKLM "Software\Classes\.mpga\OpenWithProgids" "${PROG_ID_MPGA}"
    DeleteRegValue HKLM "Software\Classes\.voc\OpenWithProgids" "${PROG_ID_VOC}"
    DeleteRegValue HKLM "Software\Classes\.wav\OpenWithProgids" "${PROG_ID_WAV}"
    DeleteRegValue HKLM "Software\Classes\.wma\OpenWithProgids" "${PROG_ID_WMA}"

    DeleteRegValue HKLM "Software\Classes\.avi\OpenWithProgids" "${PROG_ID_AVI}"
    DeleteRegValue HKLM "Software\Classes\.bik\OpenWithProgids" "${PROG_ID_BIK}"
    DeleteRegValue HKLM "Software\Classes\.mkv\OpenWithProgids" "${PROG_ID_MKV}"
    DeleteRegValue HKLM "Software\Classes\.mov\OpenWithProgids" "${PROG_ID_MOV}"
    DeleteRegValue HKLM "Software\Classes\.mp4\OpenWithProgids" "${PROG_ID_MP4}"
    DeleteRegValue HKLM "Software\Classes\.mp4v\OpenWithProgids" "${PROG_ID_MP4V}"
    DeleteRegValue HKLM "Software\Classes\.mpeg\OpenWithProgids" "${PROG_ID_MPEG}"
    DeleteRegValue HKLM "Software\Classes\.mpeg1\OpenWithProgids" "${PROG_ID_MPEG1}"
    DeleteRegValue HKLM "Software\Classes\.mpeg2\OpenWithProgids" "${PROG_ID_MPEG2}"
    DeleteRegValue HKLM "Software\Classes\.mpeg4\OpenWithProgids" "${PROG_ID_MPEG4}"
    DeleteRegValue HKLM "Software\Classes\.mpg\OpenWithProgids" "${PROG_ID_MPG}"
    DeleteRegValue HKLM "Software\Classes\.ogg\OpenWithProgids" "${PROG_ID_OGG}"
    DeleteRegValue HKLM "Software\Classes\.webm\OpenWithProgids" "${PROG_ID_WEBM}"
    DeleteRegValue HKLM "Software\Classes\.wmv\OpenWithProgids" "${PROG_ID_WMV}"

    DeleteRegValue HKLM "Software\Classes\.b4s\OpenWithProgids" "${PROG_ID_B4S}"
    DeleteRegValue HKLM "Software\Classes\.m3u\OpenWithProgids" "${PROG_ID_M3U}"
    DeleteRegValue HKLM "Software\Classes\.m3u8\OpenWithProgids" "${PROG_ID_M3U8}"
    DeleteRegValue HKLM "Software\Classes\.pls\OpenWithProgids" "${PROG_ID_PLS}"
    DeleteRegValue HKLM "Software\Classes\.wpl\OpenWithProgids" "${PROG_ID_WPL}"
    DeleteRegValue HKLM "Software\Classes\.xspf\OpenWithProgids" "${PROG_ID_XSPF}"

    # Clean up Capabilities registry trees
    DeleteRegKey HKLM "Software\Clients\Media\${APP_NAME}"
    DeleteRegValue HKLM "Software\RegisteredApplications" "${APP_NAME}"

    # Completely delete app registry keys
    DeleteRegKey HKLM "Software\Classes\${APP_NAME}"

    # Notify Windows Shell API of changes
    System::Call 'shell32::SHChangeNotify(i 0x08000000, i 0, i 0, i 0)'
SectionEnd
