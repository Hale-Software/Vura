cmake_minimum_required(VERSION 3.28...3.30)

find_package(Qt6 COMPONENTS Widgets)

add_library(libvura SHARED)
add_library(VURA::libvura ALIAS libvura)

configure_file(version.c.in version.c)
configure_file(vuraconfig.h.in "${CMAKE_BINARY_DIR}/config/vuraconfig.h" @ONLY)

target_sources(
        libvura PRIVATE
        libvura.c
        libvura.h
)

target_include_directories(libvura PUBLIC "${CMAKE_BINARY_DIR}/config" "${CMAKE_CURRENT_SOURCE_DIR}")
