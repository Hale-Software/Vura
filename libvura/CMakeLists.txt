cmake_minimum_required(VERSION 3.28...3.30)

find_package(Qt6 COMPONENTS Widgets)

add_library(libvura SHARED)
add_library(VURA::libvura ALIAS libvura)

#configure_file(vuraconfig.h.in "${CMAKE_BINARY_DIR}/config/vuraconfig.h" @ONLY)

target_sources(
        libvura PRIVATE
        constants.cpp
        constants.h
        libvura.cpp
        libvura.h
)

target_sources(
        libvura PRIVATE
        filesystem/applicationdatafile.cpp
        filesystem/applicationdatafile.h
        filesystem/hotkeysfile.cpp
        filesystem/hotkeysfile.h
        filesystem/playlistfile.cpp
        filesystem/playlistfile.h
        filesystem/videomarkersfile.cpp
        filesystem/videomarkersfile.h
        filesystem/vuraprojectfile.cpp
        filesystem/vuraprojectfile.h
)

target_sources(
        libvura PRIVATE
        util/logger.cpp
        util/logger.h
)

target_link_libraries(
        libvura PRIVATE
        Qt6::Widgets
)

target_include_directories(libvura PUBLIC "${CMAKE_BINARY_DIR}/config" "${CMAKE_CURRENT_SOURCE_DIR}")
