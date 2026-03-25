cmake_minimum_required(VERSION 3.28...3.30)

find_package(Qt6 COMPONENTS Widgets Network)

add_library(libvura SHARED)
add_library(VURA::libvura ALIAS libvura)

target_sources(
        libvura
        PRIVATE
        libvura.cpp
        libvura.h
)

target_link_libraries(
        libvura PRIVATE
        Qt6::Widgets
        Qt6::Network
)
