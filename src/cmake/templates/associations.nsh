; Macro to register a file extension
!macro RegisterExtension EXT TYPE DESC
  WriteRegStr HKCR "${EXT}" "" "${TYPE}"
  WriteRegStr HKCR "${TYPE}" "" "${DESC}"
  ; Set the default icon to the application's first icon
  WriteRegStr HKCR "${TYPE}\DefaultIcon" "" "$INSTDIR\vura64.exe,0"
  ; Set the command to open the file
  WriteRegStr HKCR "${TYPE}\shell\open\command" "" '"$INSTDIR\vura64.exe" "%1"'
!macroend

; Macro to unregister a file extension
!macro UnregisterExtension EXT TYPE
  DeleteRegKey HKCR "${EXT}"
  DeleteRegKey HKCR "${TYPE}"
!macroend

; ---------------------------------------------------------
; INSTALLER SECTIONS (Creates the Tree View)
; ---------------------------------------------------------
SectionGroup "File Associations" SecGroupAssociations

  SectionGroup "Video Files"
    Section ".mp4"
      !insertmacro RegisterExtension ".mp4" "Vura.Video" "Vura Video File"
    SectionEnd

    Section ".mkv"
      !insertmacro RegisterExtension ".mkv" "Vura.Video" "Vura Video File"
    SectionEnd
  SectionGroupEnd

  SectionGroup "Audio Files"
    Section ".mp3"
      !insertmacro RegisterExtension ".mp3" "Vura.Audio" "Vura Audio File"
    SectionEnd

    Section ".wav"
      !insertmacro RegisterExtension ".wav" "Vura.Audio" "Vura Audio File"
    SectionEnd
  SectionGroupEnd

SectionGroupEnd

; Hidden section to refresh Windows Explorer icons immediately after install
Section "-RefreshShell"
  System::Call 'shell32.dll::SHChangeNotify(i 0x08000000, i 0, i 0, i 0)'
SectionEnd

; ---------------------------------------------------------
; UNINSTALLER SECTIONS (Cleans up the Registry)
; ---------------------------------------------------------
Section "un.RemoveAssociations"
  !insertmacro UnregisterExtension ".mp4" "Vura.Video"
  !insertmacro UnregisterExtension ".mkv" "Vura.Video"
  !insertmacro UnregisterExtension ".mp3" "Vura.Audio"
  !insertmacro UnregisterExtension ".wav" "Vura.Audio"

  ; Refresh shell icons on uninstall
  System::Call 'shell32.dll::SHChangeNotify(i 0x08000000, i 0, i 0, i 0)'
SectionEnd