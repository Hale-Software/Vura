find_package(Qt6 REQUIRED COMPONENTS Widgets MultimediaWidgets Network OpenGLWidgets Svg)

target_link_libraries(
        vura PRIVATE
        Qt6::Widgets
        Qt6::MultimediaWidgets
        Qt6::Network
        Qt6::OpenGLWidgets
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
        forms/SettingsWindow.ui
        forms/TitleBar.ui
        forms/UpdateDialog.ui
        forms/VideoControlWidget.ui
        forms/VideoSliderWidget.ui
        forms/VuraMainWindow.ui
        forms/WhatsNewDialog.ui
)

# OS-Specific system libraries linking
if(WIN32)
    target_link_libraries(vura PRIVATE Opengl32)
elseif(APPLE)
    # Target macOS native OpenGL Framework wrapper
    find_library(OPENGL_LIBRARY OpenGL REQUIRED)
    target_link_libraries(vura PRIVATE ${OPENGL_LIBRARY})
elseif(UNIX AND NOT APPLE)
    target_link_libraries(vura PRIVATE GL)
endif()
