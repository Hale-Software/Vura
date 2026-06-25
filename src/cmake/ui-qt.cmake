find_package(Qt6 REQUIRED Widgets MultimediaWidgets Network Svg)

if(OS_LINUX OR OS_FREEBSD OR OS_OPENBSD)
    find_package(Qt6 REQUIRED Gui DBus)
endif()

target_link_libraries(
        vura PRIVATE
        Qt6::Widgets
        Qt6::MultimediaWidgets
        Qt6::Network
        Qt6::Svg
)

set_target_properties(
        vura
        PROPERTIES AUTOMOC ON
                   AUTOUIC ON
                   AUTORCC ON
                   AUTOGEN_PARALLEL AUTO
                   AUTOUIC_SEARCH_PATHS forms
)

set_property(TARGET vura APPEND PROPERTY AUTOUIC_SEARCH_PATHS forms)

target_sources(
        vura
        PRIVATE
        forms/vura.qrc
        forms/AboutDialog.ui
        forms/ConvertMediaDialog.ui
        forms/ErrorDialog.ui
        forms/FeedbackDialog.ui
        forms/HelpDialog.ui
        forms/HotkeyEditWidget.ui
        forms/LogUploadDialog.ui
        forms/LogViewerDialog.ui
        forms/mainwindow.ui
        forms/MarkerDialog.ui
        forms/MarkerEditDialog.ui
        forms/MediaInformationDialog.ui
        forms/MissingFilesDialog.ui
        forms/PermissionsDialog.ui
        forms/SettingsDialog.ui
        forms/TitleBar.ui
        forms/UpdateDialog.ui
        forms/VideoControlWidget.ui
        forms/VideoSliderWidget.ui
        forms/VuraMainWindow.ui
        forms/WhatsNewDialog.ui
)

qt_add_ios_ffmpeg_libraries(vura)
