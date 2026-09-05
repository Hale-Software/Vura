cmake_minimum_required(VERSION 3.16)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network MultimediaWidgets OpenGLWidgets Sql)

add_library(libvura STATIC)
add_library(VURA::libvura ALIAS libvura)

configure_file(libvura/config.h.in libvura/config.h)

# Root Directory
target_sources(
        libvura
        PUBLIC
        libvura/constants.h
        libvura/libvura.h
        libvura/helpers.h
        libvura/hotkeys.h
        libvura/settings.h
        libvura/settings-manager.h
        libvura/theme-manager.h

        PRIVATE
        libvura/constants.cpp
        libvura/libvura.cpp
        libvura/defs.h
        libvura/helpers.cpp
        libvura/hotkeys.cpp
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
    libvura/io/video-converter.h

    PRIVATE
    libvura/io/database-manager.cpp
    libvura/io/remuxer-thread.cpp
    libvura/io/video-converter.cpp
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
        libvura/media-engine/audio-decoder.h
        libvura/media-engine/audio-output.h
        libvura/media-engine/decoder-thread.h
        libvura/media-engine/demuxer-thread.h
        libvura/media-engine/fragment.glsl
        libvura/media-engine/frame-queue.h
        libvura/media-engine/media-engine.h
        libvura/media-engine/packet-queue.h
        libvura/media-engine/vertex.glsl
        libvura/media-engine/video-widget.h

        PRIVATE
        libvura/media-engine/audio-decoder.cpp
        libvura/media-engine/audio-output.cpp
        libvura/media-engine/decoder-thread.cpp
        libvura/media-engine/demuxer-thread.cpp
        libvura/media-engine/frame-queue.cpp
        libvura/media-engine/media-engine.cpp
        libvura/media-engine/packet-queue.cpp
        libvura/media-engine/video-widget.cpp
)

# models Directory
target_sources(
        libvura
        PUBLIC
        libvura/models/hotkey-model.h
        libvura/models/log-message.h
        libvura/models/metadata.h
        libvura/models/playlist-item.h
        libvura/models/playlist-model.cpp
        libvura/models/playlist-model.h
        libvura/models/version.h
        libvura/models/video-marker-record.h

        PRIVATE
        libvura/models/hotkey-model.cpp
)

# playback Directory
target_sources(
    libvura
    PUBLIC
    libvura/playback/playback-controller.h

    PRIVATE
    libvura/playback/playback-controller.cpp
)

# playlist Directory
target_sources(
        libvura
        PUBLIC
        libvura/playlist/playlist-controller.h
        libvura/playlist/playlist-delegate.h

        PRIVATE
        libvura/playlist/playlist-controller.cpp
        libvura/playlist/playlist-delegate.cpp
)

# util Directory
target_sources(
        libvura
        PUBLIC
        libvura/util/crash-reporter.h
        libvura/util/crypto.h
        libvura/util/single-instance-controller.h

        PRIVATE
        libvura/util/crash-reporter.cpp
        libvura/util/crypto.cpp
        libvura/util/single-instance-controller.cpp
)

# video-marker Directory
target_sources(
        libvura
        PUBLIC
        libvura/video-marker/video-marker-controller.h
        libvura/video-marker/video-markers.h
        libvura/video-marker/VideoMarker.h

        PRIVATE
        libvura/video-marker/video-marker-controller.cpp
        libvura/video-marker/video-markers.cpp
        libvura/video-marker/VideoMarker.cpp
)

target_link_libraries(
        libvura PRIVATE
        Qt6::Widgets
        Qt6::Network
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

# Platform specific files
if(WIN32)
    target_sources(
        libvura
        PUBLIC
        libvura/platform/platform.h
        libvura/platform/updater.h
        libvura/platform/windows/crash-handler.h
        libvura/platform/windows/keyboard-handler.h

        PRIVATE
        libvura/platform/platform-windows.cpp
        libvura/platform/updater.cpp
        libvura/platform/windows/crash-handler.cpp
        libvura/platform/windows/keyboard-handler.cpp
    )

    target_link_libraries(libvura PRIVATE dbghelp ole32 oleaut32 wbemuuid)

else()
    target_sources(
        libvura
        PUBLIC
        libvura/platform/platform.h
        libvura/platform/linux/crash-handler.h
        libvura/platform/macos/crash-handler.h

        PRIVATE
        libvura/platform/platform-linux.cpp
        libvura/platform/platform-macos.cpp
        libvura/platform/linux/crash-handler.cpp
        libvura/platform/macos/crash-handler.cpp
    )
endif()

target_include_directories(libvura PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/deps/ffmpeg/include)
