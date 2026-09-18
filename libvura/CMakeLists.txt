cmake_minimum_required(VERSION 3.16)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network Multimedia MultimediaWidgets OpenGLWidgets Sql)

add_library(libvura STATIC)
add_library(VURA::libvura ALIAS libvura)

configure_file(libvura/config.h.in libvura/config.h)

# Root Directory
target_sources(
        libvura
        PUBLIC
        libvura/constants.h
        libvura/enums.h
        libvura/helpers.h
        libvura/hotkeys.h
        libvura/libvura.h
        libvura/media-controller.h
        libvura/settings.h
        libvura/settings-manager.h
        libvura/theme-manager.h

        PRIVATE
        libvura/constants.cpp
        libvura/defs.h
        libvura/helpers.cpp
        libvura/hotkeys.cpp
        libvura/libvura.cpp
        libvura/media-controller.cpp
        libvura/settings.cpp
        libvura/settings-manager.cpp
        libvura/theme-manager.cpp
)

# exceptions Directory
target_sources(
    libvura
    PUBLIC
    libvura/exceptions/error-service.h
    libvura/exceptions/error-types.h
)

# hotkeys Directory
target_sources(
    libvura
    PUBLIC
    libvura/hotkeys/global-hotkey-host.h
    libvura/hotkeys/hotkey-manager.h

    PRIVATE
    libvura/hotkeys/global-hotkey-host.cpp
    libvura/hotkeys/hotkey-manager.cpp
)

# io Directory
target_sources(
    libvura
    PUBLIC
    libvura/io/database-manager.h
    libvura/io/remuxer-thread.h
    #libvura/io/video-converter.h

    PRIVATE
    libvura/io/database-manager.cpp
    libvura/io/remuxer-thread.cpp
    #libvura/io/video-converter.cpp
)

# logging Directory
target_sources(
        libvura
        PUBLIC
        libvura/logging/categories.h
        libvura/logging/log-uploader.h
        libvura/logging/logger.h

        PRIVATE
        libvura/logging/categories.cpp
        libvura/logging/log-uploader.cpp
        libvura/logging/logger.cpp
)

# media-engine Directory
target_sources(
    libvura
    PUBLIC
    libvura/media-engine/engine.h
    libvura/media-engine/engine-factory.h
    #libvura/media-engine/mpv-engine.h
    libvura/media-engine/null-engine.h
    libvura/media-engine/qt-engine.h
    libvura/media-engine/video-output.h
    libvura/media-engine/video-stage.h

    PRIVATE
    libvura/media-engine/engine.cpp
    libvura/media-engine/engine-factory.cpp
    #libvura/media-engine/mpv-engine.cpp
    libvura/media-engine/null-engine.cpp
    libvura/media-engine/qt-engine.cpp
    libvura/media-engine/video-stage.cpp
)

# models Directory
target_sources(
        libvura
        PUBLIC
        libvura/models/hotkey-model.h
        libvura/models/log-message.h
        libvura/models/metadata.h
        libvura/models/playlist.h
        libvura/models/subtitle-cue.h
        libvura/models/types.h
        libvura/models/version.h
        libvura/models/video-marker-record.h

        PRIVATE
        libvura/models/hotkey-model.cpp
        libvura/models/playlist.cpp
        libvura/models/types.cpp
)

# opengl Directory
#target_sources(
#        libvura
#        PUBLIC
#        libvura/media-engine/audio-decoder.h
#        libvura/media-engine/audio-output.h
#        libvura/media-engine/decoder-thread.h
#        libvura/media-engine/demuxer-thread.h
#        libvura/media-engine/fragment.glsl
#        libvura/media-engine/frame-queue.h
#        libvura/media-engine/media-engine.h
#        libvura/media-engine/packet-queue.h
#        libvura/media-engine/vertex.glsl
#        libvura/media-engine/video-widget.h

#        PRIVATE
#        libvura/media-engine/audio-decoder.cpp
#        libvura/media-engine/audio-output.cpp
#        libvura/media-engine/decoder-thread.cpp
#        libvura/media-engine/demuxer-thread.cpp
#        libvura/media-engine/frame-queue.cpp
#        libvura/media-engine/media-engine.cpp
#        libvura/media-engine/packet-queue.cpp
#        libvura/media-engine/video-widget.cpp
#)

# subtitles Directory
target_sources(
    libvura
    PUBLIC
    libvura/subtitles/subtitle-track.h

    PRIVATE
    libvura/subtitles/subtitle-track.cpp
)

# util Directory
target_sources(
        libvura
        PUBLIC
        libvura/util/crash-reporter.h
        libvura/util/crypto.h
        libvura/util/resume-store.h
        libvura/util/single-instance-controller.h

        PRIVATE
        libvura/util/crash-reporter.cpp
        libvura/util/crypto.cpp
        libvura/util/resume-store.cpp
        libvura/util/single-instance-controller.cpp
)

target_link_libraries(
        libvura PRIVATE
        Qt6::Widgets
        Qt6::Network
        Qt6::Multimedia
        Qt6::MultimediaWidgets
        Qt6::OpenGLWidgets
        Qt6::Sql
        ${CMAKE_SOURCE_DIR}/deps/ffmpeg/lib/avformat.lib
        ${CMAKE_SOURCE_DIR}/deps/ffmpeg/lib/avcodec.lib
        ${CMAKE_SOURCE_DIR}/deps/ffmpeg/lib/avfilter.lib
        ${CMAKE_SOURCE_DIR}/deps/ffmpeg/lib/avutil.lib
        ${CMAKE_SOURCE_DIR}/deps/ffmpeg/lib/swscale.lib
        ${CMAKE_SOURCE_DIR}/deps/ffmpeg/lib/swresample.lib
)

target_compile_definitions(libvura PUBLIC MEDIA_HAVE_QTMULTIMEDIA)

# Platform specific files
if(WIN32)
    target_sources(
        libvura
        PUBLIC
        libvura/platform/platform.h
        libvura/platform/updater.h
        libvura/platform/windows/crash-handler.h
        libvura/platform/windows/keyboard-handler.h
        libvura/platform/windows/sleep-inhibitor.h

        PRIVATE
        libvura/platform/platform-windows.cpp
        libvura/platform/updater.cpp
        libvura/platform/windows/crash-handler.cpp
        libvura/platform/windows/keyboard-handler.cpp
        libvura/platform/windows/sleep-inhibitor.cpp
    )

    target_link_libraries(libvura PRIVATE dbghelp ole32 oleaut32 wbemuuid)

elseif(APPLE)

    target_sources(
        libvura
        PUBLIC
        libvura/platform/platform.h
        libvura/platform/macos/crash-handler.h
        libvura/platform/macos/sleep-inhibitor.h

        PRIVATE
        libvura/platform/platform-macos.cpp
        libvura/platform/macos/crash-handler.cpp
        libvura/platform/macos/sleep-inhibitor.cpp
    )

elseif(UNIX)
    target_sources(
        libvura
        PUBLIC
        libvura/platform/platform.h
        libvura/platform/linux/crash-handler.h
        libvura/platform/linux/sleep-inhibitor.h

        PRIVATE
        libvura/platform/platform-linux.cpp
        libvura/platform/linux/crash-handler.cpp
        libvura/platform/linux/sleep-inhibitor.cpp
    )
endif()

target_include_directories(libvura PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/deps/ffmpeg/include)
