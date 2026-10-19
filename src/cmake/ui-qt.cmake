
target_link_libraries(
    vura PRIVATE
    Qt6::Widgets
    Qt6::Network
    Qt6::Sql
)

if (VURA_ENABLE_QTMULTIMEDIA)
    target_link_libraries(
        vura PRIVATE
        Qt6::Multimedia
        Qt6::MultimediaWidgets
    )

    target_compile_definitions(vura PRIVATE VURA_HAVE_QTMULTIMEDIA)
endif ()

if (VURA_ENABLE_MPV)
    target_link_libraries(
        vura PRIVATE
        PkgConfig::MPV
    )

    target_compile_definitions(vura PRIVATE VURA_HAVE_MPV)

    if(WIN32)
        target_link_libraries(vura PRIVATE Opengl32)
    elseif(APPLE)
        find_library(OPENGL_LIBRARY OpenGL REQUIRED)
        target_link_libraries(vura PRIVATE ${OPENGL_LIBRARY})
    elseif(UNIX AND NOT APPLE)
        target_link_libraries(vura PRIVATE GL)
    endif()
endif ()

if (VURA_ENABLE_OPENGL)
    target_link_libraries(
        vura PRIVATE
        Qt6::Multimedia
        Qt6::MultimediaWidgets
        Qt6::OpenGLWidgets
    )

    target_compile_definitions(vura PRIVATE VURA_HAVE_OPENGL)

    if(WIN32)
        target_link_libraries(vura PRIVATE Opengl32)
    elseif(APPLE)
        find_library(OPENGL_LIBRARY OpenGL REQUIRED)
        target_link_libraries(vura PRIVATE ${OPENGL_LIBRARY})
    elseif(UNIX AND NOT APPLE)
        target_link_libraries(vura PRIVATE GL)
    endif()
endif ()

target_sources(
    vura PRIVATE
    forms/vura.qrc
    forms/AboutDialog.ui
    forms/ContinuePlaybackWidget.ui
    forms/ConvertMediaDialog.ui
    forms/ErrorDialog.ui
    forms/FeedbackDialog.ui
    forms/HelpDialog.ui
    forms/LogUploadDialog.ui
    forms/LogViewerDialog.ui
    forms/MarkerEditDialog.ui
    forms/MediaInformationDialog.ui
    forms/MissingFilesDialog.ui
    forms/PlaylistWidget.ui
    forms/SettingsDialog.ui
    forms/SideDockWidget.ui
    forms/UpdateDialog.ui
    forms/VideoControlWidget.ui
    forms/VideoSliderWidget.ui
    forms/VuraMainWindow.ui
    forms/WhatsNewDialog.ui
)
