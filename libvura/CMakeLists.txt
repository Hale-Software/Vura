cmake_minimum_required(VERSION 3.16)

find_package(Qt6 REQUIRED Core Widgets Network MultimediaWidgets)

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
        libvura/markermanager.h
        libvura/mediacontroller.h
        libvura/datafile-handler.h
        libvura/error.h
        libvura/ErrorTypes.h
        libvura/ErrorService.h
        libvura/helpers.h
        libvura/hotkeys.h
        libvura/settings.h

        PRIVATE
        libvura/constants.cpp
        libvura/filemanager.cpp
        libvura/libvura.cpp
        libvura/markermanager.cpp
        libvura/mediacontroller.cpp
        libvura/datafile-handler.cpp
        libvura/defs.h
        libvura/error.cpp
        libvura/helpers.cpp
        libvura/hotkeys.cpp
        libvura/settings.cpp
)

# data Directory
target_sources(
        libvura
        PUBLIC
        libvura/data/video-markers.h

        PRIVATE
        libvura/data/video-markers.cpp
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
        libvura/models/log-viewer-model.h
        libvura/models/logmessages.h
        libvura/models/vura-playlistmodel.h

        PRIVATE
        libvura/models/log-viewer-model.cpp
        libvura/models/logmessages.cpp
        libvura/models/vura-playlistmodel.cpp
)

# playlist Directory
target_sources(
        libvura
        PUBLIC
        libvura/playlist/MediaItem.h
        libvura/playlist/PlaylistModel.h
        libvura/playlist/PlaylistDelegate.h
)

# util Directory
target_sources(
        libvura
        PUBLIC
        libvura/util/blogger.h
        libvura/util/messagebox.h
        libvura/util/playlistmanager.h
        libvura/util/singleinstance.h

        PRIVATE
        libvura/util/blogger.cpp
        libvura/util/messagebox.cpp
        libvura/util/playlistmanager.cpp
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

target_link_libraries(libvura PRIVATE Qt6::Core Qt6::Widgets Qt6::Network Qt6::MultimediaWidgets)
target_include_directories(libvura PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
