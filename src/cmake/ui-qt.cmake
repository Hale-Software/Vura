find_package(Qt6 REQUIRED Widgets MultimediaWidgets Network Svg)

if(OS_LINUX OR OS_FREEBSD OR OS_OPENBSD)
    find_package(Qt6 REQUIRED Gui DBus)
endif()

target_link_libraries(
        vura
        PRIVATE Qt6::Widgets Qt6::MultimediaWidgets Qt6::Network Qt6::Svg
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
        forms/mainwindow.ui
        forms/settingswindow.ui
        forms/videocontrolwidget.ui
        forms/logviewer.ui
        forms/MarkerEditDialog.ui
        forms/missingfiles.ui
        forms/permissions.ui
        forms/updatewindow.ui
        forms/whatsnew.ui
        forms/mediainformation.ui
        forms/testwindow.ui
        forms/ContinuePlaybackRibbon.ui
        forms/helpdialog.ui
        forms/LogUploadDialog.ui
        forms/FeedbackDialog.ui
        forms/markerdialog.ui
        forms/convertmediadialog.ui
)

qt_add_ios_ffmpeg_libraries(vura)
