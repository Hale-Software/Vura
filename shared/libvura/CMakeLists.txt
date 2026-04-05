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
        vura-data.h
        vura-error.h
        vura-overloads.h
        #vura-file-handler.h
        vura-helpers.h
        vura-hotkeys.h
        vura-serializer.h
        vura-settings.h
        vura-startup.h

        PRIVATE
        constants.cpp
        libvura.cpp
        vura-data.cpp
        vura-defs.h
        vura-error.cpp
        #vura-file-handler.cpp
        vura-helpers.cpp
        vura-hotkeys.cpp
        vura-serializer.cpp
        vura-settings.cpp
        vura-startup.cpp
)

# Media-io Directory
target_sources(
        libvura
        PUBLIC
        media-io/media-functions.h

        PRIVATE
        media-io/media-functions.cpp
)

# Models Directory
target_sources(
        libvura
        PUBLIC
        models/logmessages.h

        PRIVATE
        models/logmessages.cpp
)

# Util Directory
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

# Util/Windows Directory
#target_sources(
#        libvura
#        PUBLIC
#        util/windows/windowskeyboardhandler.h

#        PRIVATE
#        util/windows/windowskeyboardhandler.cpp
#)

target_link_libraries(libvura PRIVATE Qt6::Core Qt6::Widgets Qt6::Network Qt6::MultimediaWidgets)
target_include_directories(libvura INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
