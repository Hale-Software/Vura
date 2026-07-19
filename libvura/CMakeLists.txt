cmake_minimum_required(VERSION 3.16)

set(FFMPEG_DIR "C:\\Users\\halea\\bin\\ffmpeg-8.1.1-full_build-shared")

find_package(Qt6 REQUIRED COMPONENTS Widgets Network MultimediaWidgets OpenGLWidgets)

add_library(libvura STATIC)
add_library(VURA::libvura ALIAS libvura)

configure_file(libvura/config.h.in libvura/config.h)

# Root Directory
target_sources(
        libvura
        PUBLIC
        libvura/constants.h
        libvura/filemanager.h
        libvura/libvura.h
        libvura/logger.h
        libvura/markermanager.h
        libvura/mediacontroller.h
        libvura/datafile-handler.h
        libvura/error.h
        libvura/ErrorTypes.h
        libvura/ErrorService.h
        libvura/helpers.h
        libvura/hotkeys.h
        libvura/settings.h
        libvura/settings-manager.h
        libvura/theme-manager.h

        PRIVATE
        libvura/constants.cpp
        libvura/filemanager.cpp
        libvura/libvura.cpp
        libvura/logger.cpp
        libvura/markermanager.cpp
        libvura/mediacontroller.cpp
        libvura/datafile-handler.cpp
        libvura/defs.h
        libvura/error.cpp
        libvura/helpers.cpp
        libvura/hotkeys.cpp
        libvura/settings.cpp
        libvura/settings-manager.cpp
        libvura/theme-manager.cpp
)

# data Directory
target_sources(
        libvura
        PUBLIC
        libvura/data/version-info.h
        libvura/data/video-markers.h
        libvura/data/VideoMarker.h

        PRIVATE
        libvura/data/video-markers.cpp
        libvura/data/VideoMarker.cpp
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
        libvura/media-engine/packet-queue.h
        libvura/media-engine/vertex.glsl
        libvura/media-engine/video-widget.h

        PRIVATE
        libvura/media-engine/audio-decoder.cpp
        libvura/media-engine/audio-output.cpp
        libvura/media-engine/decoder-thread.cpp
        libvura/media-engine/demuxer-thread.cpp
        libvura/media-engine/frame-queue.cpp
        libvura/media-engine/packet-queue.cpp
        libvura/media-engine/video-widget.cpp
)

# media-io Directory
target_sources(
        libvura
        PUBLIC
        libvura/media-io/media-functions.h

        PRIVATE
        libvura/media-io/media-functions.cpp
)

# models Directory
target_sources(
        libvura
        PUBLIC
        libvura/models/playlist-item.h
)

# playlist Directory
target_sources(
        libvura
        PUBLIC
        libvura/playlist/playlist-model.h
        libvura/playlist/playlist-delegate.h

        PRIVATE
        libvura/playlist/playlist-model.cpp
        libvura/playlist/playlist-delegate.cpp
)

# util Directory
target_sources(
        libvura
        PUBLIC
        libvura/util/blogger.h
        libvura/util/messagebox.h
        libvura/util/singleinstance.h

        PRIVATE
        libvura/util/blogger.cpp
        libvura/util/messagebox.cpp
        libvura/util/singleinstance.cpp
)

# util/windows Directory
#target_sources(
#        libvura
#        PUBLIC
#        util/windows/windowskeyboardhandler.h

#        PRIVATE
#        util/windows/windowskeyboardhandler.cpp
#)

target_link_libraries(
        libvura PRIVATE
        Qt6::Widgets
        Qt6::Network
        Qt6::MultimediaWidgets
        Qt6::OpenGLWidgets
        ${FFMPEG_DIR}/lib/avformat.lib
        ${FFMPEG_DIR}/lib/avcodec.lib
        ${FFMPEG_DIR}/lib/avfilter.lib
        ${FFMPEG_DIR}/lib/avutil.lib
        ${FFMPEG_DIR}/lib/swscale.lib
        ${FFMPEG_DIR}/lib/swresample.lib
)

target_include_directories(libvura PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${FFMPEG_DIR}/include)
