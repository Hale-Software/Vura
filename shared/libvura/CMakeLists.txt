cmake_minimum_required(VERSION 3.16)

find_package(Qt6 REQUIRED Core Widgets Network MultimediaWidgets)

add_library(libvura STATIC)
add_library(VURA::libvura ALIAS libvura)

configure_file(vura-config.h.in vura-config.h)

# Root Directory
target_sources(
        libvura
        PUBLIC
        constants.h
        libvura.h
        vura-datafile-handler.h
        vura-error.h
        vura-helpers.h
        vura-hotkeys.h
        vura-settings.h

        PRIVATE
        constants.cpp
        libvura.cpp
        vura-datafile-handler.cpp
        vura-defs.h
        vura-error.cpp
        vura-helpers.cpp
        vura-hotkeys.cpp
        vura-settings.cpp
)

# data Directory
target_sources(
        libvura
        PUBLIC
        data/video-markers.h

        PRIVATE
        data/video-markers.cpp
)

# media-io Directory
target_sources(
        libvura
        PUBLIC
        media-io/media-functions.h

        PRIVATE
        media-io/media-functions.cpp
)

# models Directory
target_sources(
        libvura
        PUBLIC
        models/log-viewer-model.h
        models/logmessages.h

        PRIVATE
        models/log-viewer-model.cpp
        models/logmessages.cpp
)

# util Directory
target_sources(
        libvura
        PUBLIC
        util/blogger.h
        util/messagebox.h
        util/singleinstance.h
        PRIVATE
        util/blogger.cpp
        util/messagebox.cpp
        util/singleinstance.cpp
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
target_include_directories(libvura INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
