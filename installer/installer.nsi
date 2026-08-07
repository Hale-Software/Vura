# ============================================================================
# NSIS Installer Script for Vura (Qt/FFmpeg Media Player)
# ============================================================================
# Refactor notes (see accompanying summary for full details):
#   - File-association registration/unregistration is now driven from ONE
#     list per category (macros ending in _FILE_ASSOCS below). Each line is
#     the single source of truth for that extension: it is used to both
#     generate the installer's per-extension checkbox AND to remove that
#     exact registration on uninstall. There is no second place to keep in
#     sync, which was the root cause of two bugs in the previous version:
#       1. Every association wrote its Capabilities\FileAssociations value
#          under the literal name "." instead of the extension, so only the
#          last-registered extension ever showed up in Windows' Default Apps.
#       2. The uninstaller only knew how to remove ~25 of the ~60 registered
#          ProgIDs, leaving orphaned registry entries after uninstall.
#   - SetShellVarContext all added so Start Menu/Desktop shortcuts are
#     written to (and removed from) the all-users location, matching the
#     all-users $PROGRAMFILES64 install dir.
#   - "Launch Vura" on the finish page now launches de-elevated (via the
#     explorer.exe relay trick) instead of inheriting the installer's admin
#     token, since a media player has no reason to run elevated.
#   - APP_NAME is used consistently instead of a hardcoded "Vura" string.
# ============================================================================

!ifndef VERSION
  !define VERSION "0.0.0-dev"
!endif

!define APP_NAME "Vura"
!define COMP_NAME "Hale Software"
!define EXE_NAME "vura.exe"
!define PROG_ID_PREFIX "CustomMediaPlayer.AssocFile"
!define UNINSTALL_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"

Name "${APP_NAME}"
OutFile "vura-${VERSION}-windows.exe"
InstallDir "$PROGRAMFILES64\${APP_NAME}"
SetCompressor /SOLID lzma

RequestExecutionLevel admin

Var StartMenuFolder

!include "MUI2.nsh"

# Interface Settings
!define MUI_ABORTWARNING
!define MUI_LANGDLL_ALLLANGUAGES
!define MUI_ICON "deploy\dist_windows\assets\icons\vura.ico"
!define MUI_UNICON "deploy\dist_windows\assets\icons\vura.ico"

!define MUI_LANGDLL_REGISTRY_ROOT "HKCU"
!define MUI_LANGDLL_REGISTRY_KEY "Software\${APP_NAME}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

# Page layout
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "deploy\dist_windows\assets\data\license.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY

!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\${APP_NAME}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder

!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_RUN "$INSTDIR\${EXE_NAME}"
!define MUI_FINISHPAGE_RUN_TEXT "Launch Vura"
!define MUI_FINISHPAGE_RUN_FUNCTION "LaunchAppUnelevated"

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
# INIT — make sure all-users shell folders are used for shortcuts, both for
# install and uninstall (fixes shortcuts being written to the invoking
# admin's per-user Start Menu/Desktop instead of the all-users one).
# ============================================================================
Function .onInit
    SetShellVarContext all
FunctionEnd

Function un.onInit
    SetShellVarContext all
FunctionEnd

# ============================================================================
# Launch the app un-elevated from the finish page.
# The installer runs elevated (RequestExecutionLevel admin), but Vura itself
# has no reason to run as admin. Relaying the launch through explorer.exe
# (which already runs at the logged-in user's integrity level) starts the
# app un-elevated, with no extra plugin dependency.
# ============================================================================
Function LaunchAppUnelevated
    Exec '"$WINDIR\explorer.exe" "$INSTDIR\${EXE_NAME}"'
FunctionEnd

# ============================================================================
# FILE ASSOCIATION MACROS
# ============================================================================
# FILEASSOC_ENTRY is the single source of truth for one extension.
#
#   - During install (UNINSTALL_MODE not defined), it expands into a
#     checkbox Section that writes all the registry keys for that
#     extension, including the Capabilities\FileAssociations value keyed
#     correctly by extension (not the literal "." from the old script).
#   - During uninstall (UNINSTALL_MODE defined before the group macros are
#     invoked again), the exact same line expands into the matching
#     registry removal instead — so install and uninstall can never drift
#     out of sync, because they're generated from one list.
# ============================================================================
!macro FILEASSOC_ENTRY EXT SUFFIX DESC
  !ifdef UNINSTALL_MODE
    DeleteRegKey HKLM "Software\Classes\${PROG_ID_PREFIX}.${SUFFIX}"
    DeleteRegValue HKLM "Software\Classes\${EXT}\OpenWithProgids" "${PROG_ID_PREFIX}.${SUFFIX}"
  !else
    Section "${EXT}"
        WriteRegStr HKLM "Software\Clients\Media\${APP_NAME}\Capabilities\FileAssociations" "${EXT}" "${PROG_ID_PREFIX}.${SUFFIX}"
        WriteRegStr HKLM "Software\Classes\${EXT}\OpenWithProgids" "${PROG_ID_PREFIX}.${SUFFIX}" ""
        WriteRegStr HKLM "Software\Classes\${PROG_ID_PREFIX}.${SUFFIX}\DefaultIcon" "" '"$INSTDIR\${EXE_NAME}",0'
        WriteRegStr HKLM "Software\Classes\${PROG_ID_PREFIX}.${SUFFIX}" "" "${DESC}"
        WriteRegStr HKLM "Software\Classes\${PROG_ID_PREFIX}.${SUFFIX}\shell\open\command" "" '"$INSTDIR\${EXE_NAME}" "%1"'
    SectionEnd
  !endif
!macroend

# ---- Application file types --------------------------------------------
!macro APP_FILE_ASSOCS
    !insertmacro FILEASSOC_ENTRY ".vhk"  "VHK"  "Vura Hotkey File"
    !insertmacro FILEASSOC_ENTRY ".vpl"  "VPL"  "Vura Playlist File"
    !insertmacro FILEASSOC_ENTRY ".vprj" "VPRJ" "Vura Project File"
    !insertmacro FILEASSOC_ENTRY ".vvm"  "VVM"  "Vura Marker File"
!macroend

# ---- Audio file types -----------------------------------------------------
!macro AUDIO_FILE_ASSOCS
    !insertmacro FILEASSOC_ENTRY ".3ga"  "3GA"  "3GA Audio File"
    !insertmacro FILEASSOC_ENTRY ".669"  "669"  "669 Module File"
    !insertmacro FILEASSOC_ENTRY ".a52"  "A52"  "A52 Audio File"
    !insertmacro FILEASSOC_ENTRY ".aac"  "AAC"  "AAC Audio File"
    !insertmacro FILEASSOC_ENTRY ".ac3"  "AC3"  "AC3 Audio File"
    !insertmacro FILEASSOC_ENTRY ".adt"  "ADT"  "ADT Audio File"
    !insertmacro FILEASSOC_ENTRY ".adts" "ADTS" "ADTS Audio File"
    !insertmacro FILEASSOC_ENTRY ".aif"  "AIF"  "AIF Audio File"
    !insertmacro FILEASSOC_ENTRY ".aifc" "AIFC" "AIFC Audio File"
    !insertmacro FILEASSOC_ENTRY ".aiff" "AIFF" "AIFF Audio File"
    !insertmacro FILEASSOC_ENTRY ".au"   "AU"   "AU Audio File"
    !insertmacro FILEASSOC_ENTRY ".amr"  "AMR"  "AMR Audio File"
    !insertmacro FILEASSOC_ENTRY ".aob"  "AOB"  "AOB Audio File"
    !insertmacro FILEASSOC_ENTRY ".ape"  "APE"  "APE Audio File"
    !insertmacro FILEASSOC_ENTRY ".caf"  "CAF"  "CAF Audio File"
    !insertmacro FILEASSOC_ENTRY ".cda"  "CDA"  "CDA Audio File"
    !insertmacro FILEASSOC_ENTRY ".dts"  "DTS"  "DTS Audio File"
    !insertmacro FILEASSOC_ENTRY ".flac" "FLAC" "FLAC Audio File"
    !insertmacro FILEASSOC_ENTRY ".it"   "IT"   "IT Module File"
    !insertmacro FILEASSOC_ENTRY ".m4a"  "M4A"  "M4A Audio File"
    !insertmacro FILEASSOC_ENTRY ".m4p"  "M4P"  "M4P Audio File"
    !insertmacro FILEASSOC_ENTRY ".mlp"  "MLP"  "MLP Audio File"
    !insertmacro FILEASSOC_ENTRY ".mod"  "MOD"  "MOD Module File"
    !insertmacro FILEASSOC_ENTRY ".mp1"  "MP1"  "MP1 Audio File"
    !insertmacro FILEASSOC_ENTRY ".mp2"  "MP2"  "MP2 Audio File"
    !insertmacro FILEASSOC_ENTRY ".mp3"  "MP3"  "MP3 Audio File"
    !insertmacro FILEASSOC_ENTRY ".mpc"  "MPC"  "MPC Audio File"
    !insertmacro FILEASSOC_ENTRY ".mpga" "MPGA" "MPGA Audio File"
    !insertmacro FILEASSOC_ENTRY ".oga"  "OGA"  "OGA Audio File"
    !insertmacro FILEASSOC_ENTRY ".oma"  "OMA"  "OMA Audio File"
    !insertmacro FILEASSOC_ENTRY ".opus" "OPUS" "OPUS Audio File"
    !insertmacro FILEASSOC_ENTRY ".qcp"  "QCP"  "QCP Audio File"
    !insertmacro FILEASSOC_ENTRY ".ra"   "RA"   "RA Audio File"
    !insertmacro FILEASSOC_ENTRY ".rmi"  "RMI"  "RMI Audio File"
    !insertmacro FILEASSOC_ENTRY ".snd"  "SND"  "SND Audio File"
    !insertmacro FILEASSOC_ENTRY ".s3m"  "S3M"  "S3M Module File"
    !insertmacro FILEASSOC_ENTRY ".spx"  "SPX"  "SPX Audio File"
    !insertmacro FILEASSOC_ENTRY ".tta"  "TTA"  "TTA Audio File"
    !insertmacro FILEASSOC_ENTRY ".voc"  "VOC"  "VOC Audio File"
    !insertmacro FILEASSOC_ENTRY ".vqf"  "VQF"  "VQF Audio File"
    !insertmacro FILEASSOC_ENTRY ".w64"  "W64"  "W64 Audio File"
    !insertmacro FILEASSOC_ENTRY ".wav"  "WAV"  "WAV Audio File"
    !insertmacro FILEASSOC_ENTRY ".wma"  "WMA"  "WMA Audio File"
    !insertmacro FILEASSOC_ENTRY ".wv"   "WV"   "WV Audio File"
    !insertmacro FILEASSOC_ENTRY ".xa"   "XA"   "XA Audio File"
    !insertmacro FILEASSOC_ENTRY ".xm"   "XM"   "XM Module File"
!macroend

# ---- Video file types -------------------------------------------------
!macro VIDEO_FILE_ASSOCS
    !insertmacro FILEASSOC_ENTRY ".3g2"   "3G2"   "3G2 Video File"
    !insertmacro FILEASSOC_ENTRY ".3gp"   "3GP"   "3GP Video File"
    !insertmacro FILEASSOC_ENTRY ".3gp2"  "3GP2"  "3GP2 Video File"
    !insertmacro FILEASSOC_ENTRY ".3gpp"  "3GPP"  "3GPP Video File"
    !insertmacro FILEASSOC_ENTRY ".amv"   "AMV"   "AMV Video File"
    !insertmacro FILEASSOC_ENTRY ".asf"   "ASF"   "ASF Video File"
    !insertmacro FILEASSOC_ENTRY ".avi"   "AVI"   "AVI Video File"
    !insertmacro FILEASSOC_ENTRY ".bik"   "BIK"   "BIK Video File"
    !insertmacro FILEASSOC_ENTRY ".dav"   "DAV"   "DAV Video File"
    !insertmacro FILEASSOC_ENTRY ".divx"  "DIVX"  "DIVX Video File"
    !insertmacro FILEASSOC_ENTRY ".drc"   "DRC"   "DRC Video File"
    !insertmacro FILEASSOC_ENTRY ".dv"    "DV"    "DV Video File"
    !insertmacro FILEASSOC_ENTRY ".m4v"   "M4V"   "M4V Video File"
    !insertmacro FILEASSOC_ENTRY ".mkv"   "MKV"   "MKV Video File"
    !insertmacro FILEASSOC_ENTRY ".mov"   "MOV"   "MOV Video File"
    !insertmacro FILEASSOC_ENTRY ".mp2v"  "MP2V"  "MP2V Video File"
    !insertmacro FILEASSOC_ENTRY ".mp4"   "MP4"   "MP4 Video File"
    !insertmacro FILEASSOC_ENTRY ".mp4v"  "MP4V"  "MP4V Video File"
    !insertmacro FILEASSOC_ENTRY ".mpeg"  "MPEG"  "MPEG Video File"
    !insertmacro FILEASSOC_ENTRY ".mpeg1" "MPEG1" "MPEG1 Video File"
    !insertmacro FILEASSOC_ENTRY ".mpeg2" "MPEG2" "MPEG2 Video File"
    !insertmacro FILEASSOC_ENTRY ".mpeg4" "MPEG4" "MPEG4 Video File"
    !insertmacro FILEASSOC_ENTRY ".mpg"   "MPG"   "MPG Video File"
    !insertmacro FILEASSOC_ENTRY ".ogg"   "OGG"   "OGG Video File"
    !insertmacro FILEASSOC_ENTRY ".webm"  "WEBM"  "WEBM Video File"
    !insertmacro FILEASSOC_ENTRY ".wmv"   "WMV"   "WMV Video File"
!macroend

# ---- Playlist / misc file types ----------------------------------------
!macro MISC_FILE_ASSOCS
    !insertmacro FILEASSOC_ENTRY ".b4s"  "B4S"  "Vura Playlist File"
    !insertmacro FILEASSOC_ENTRY ".m3u"  "M3U"  "Vura Playlist File"
    !insertmacro FILEASSOC_ENTRY ".m3u8" "M3U8" "Vura Playlist File"
    !insertmacro FILEASSOC_ENTRY ".pls"  "PLS"  "Vura Playlist File"
    !insertmacro FILEASSOC_ENTRY ".wpl"  "WPL"  "Vura Playlist File"
    !insertmacro FILEASSOC_ENTRY ".xspf" "XSPF" "Vura Playlist File"
!macroend

# ============================================================================
# INSTALLATION SECTION
# ============================================================================
Section "Media Player (Required)" SecCore
    SectionIn RO

    SetOutPath "$INSTDIR"

    # Close a running instance first so files aren't locked mid-install
    ExecWait 'taskkill /F /IM ${EXE_NAME}'

    # Store Installation Path in Registry for future reference
    WriteRegStr HKCU "Software\${APP_NAME}" "" $INSTDIR

    # Main Executable and Core Plugins
    File "deploy\dist_windows\${EXE_NAME}"
    File "deploy\dist_windows\*.dll"
    File /r "deploy\dist_windows\assets"

    # Copy Qt Runtime Directory Trees
    File /r "deploy\dist_windows\generic"
    File /r "deploy\dist_windows\iconengines"
    File /r "deploy\dist_windows\imageformats"
    File /r "deploy\dist_windows\multimedia"
    File /r "deploy\dist_windows\platforms"
    File /r "deploy\dist_windows\styles"
    File /r "deploy\dist_windows\tls"
    File /r "deploy\dist_windows\networkinformation"
    File /r "deploy\dist_windows\translations"

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

# Optional Section for File Associations — each group macro expands into one
# checkbox Section per extension, exactly as in the original script.
SectionGroup "File Associations" SecFileAssociations

    SectionGroup "Application Files" ApplicationFilesGroup
        !insertmacro APP_FILE_ASSOCS
    SectionGroupEnd

    SectionGroup "Audio Files" AudioFilesGroup
        !insertmacro AUDIO_FILE_ASSOCS
    SectionGroupEnd

    SectionGroup "Video Files" VideoFilesGroup
        !insertmacro VIDEO_FILE_ASSOCS
    SectionGroupEnd

    SectionGroup "Misc Files" MiscFilesGroup
        !insertmacro MISC_FILE_ASSOCS
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
    # Recursive fallback: catches anything left behind (e.g. a future build
    # adds a new Qt plugin directory and this list isn't updated in lockstep).
    RMDir /r "$INSTDIR"

    # Wipe system shortcuts
    Delete "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk"
    Delete "$SMPROGRAMS\${APP_NAME}\Uninstall.lnk"
    RMDir "$SMPROGRAMS\${APP_NAME}"
    Delete "$DESKTOP\${APP_NAME}.lnk"

    # Remove Windows Add/Remove configuration registry branch
    DeleteRegKey HKLM "${UNINSTALL_KEY}"
    DeleteRegKey HKCU "Software\${APP_NAME}"

    # Remove all file-association registrations. Defining UNINSTALL_MODE
    # switches FILEASSOC_ENTRY from "install checkbox Section" to "delete
    # these two registry entries" — using the exact same list that install
    # used, so nothing can be left behind or drift out of sync.
    !define UNINSTALL_MODE
    !insertmacro APP_FILE_ASSOCS
    !insertmacro AUDIO_FILE_ASSOCS
    !insertmacro VIDEO_FILE_ASSOCS
    !insertmacro MISC_FILE_ASSOCS
    !undef UNINSTALL_MODE

    # Remove context-menu registrations
    DeleteRegKey HKLM "Software\Classes\*\shell\${APP_NAME}"
    DeleteRegKey HKLM "Software\Classes\Directory\shell\${APP_NAME}"
    DeleteRegKey HKLM "Software\Classes\Directory\Background\shell\${APP_NAME}"

    # Remove custom URI scheme registration
    DeleteRegKey HKLM "Software\Classes\${APP_NAME}"

    # Clean up Capabilities registry trees
    DeleteRegKey HKLM "Software\Clients\Media\${APP_NAME}"
    DeleteRegValue HKLM "Software\RegisteredApplications" "${APP_NAME}"

    # Notify Windows Shell API of changes
    System::Call 'shell32::SHChangeNotify(i 0x08000000, i 0, i 0, i 0)'
SectionEnd
