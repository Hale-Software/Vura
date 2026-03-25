cmake_minimum_required(VERSION 3.28...3.30)

find_package(Qt6 COMPONENTS Widgets Network)

include(cmake/libvura-version.cmake)

if (OS_WINDOWS)
    include(cmake/os-windows.cmake)
    return()
endif ()

add_library(libvura SHARED)
add_library(VURA::libvura ALIAS libvura)

configure_file(version.c.in version.c)
configure_file(vuraconfig.h.in "${CMAKE_BINARY_DIR}/config/vuraconfig.h" @ONLY)

target_sources(
        libvura PRIVATE
        vura-config.h
        vura-data.c
        vura-data.h
        vura-hotkey.c
        vura-hotkey.h
        vura-hotkeys.h
        vura-interaction.h
        vura-internal.h
        vura-missing-files.c
        vura-missing-files.h
        vura-properties.c
        vura-properties.h
        vura.c
        vura.h
        vura.hpp
)

target_sources(
        libvura PRIVATE
        util/base.c
        util/base.h
        util/config-file.c
        util/config-file.h
        util/defs.h
        util/file-serializer.c
        util/file-serializer.h
        util/platform.c
        util/platform.h
        util/task.c
        util/task.h
        util/threading.h
)

target_link_libraries(
        libvura PRIVATE
        Qt6::Widgets
        Qt6::Network
        VURA::libvura-version
)

target_include_directories(
        libvura PUBLIC
        "${CMAKE_BINARY_DIR}/config"
        "${CMAKE_CURRENT_SOURCE_DIR}"
)
