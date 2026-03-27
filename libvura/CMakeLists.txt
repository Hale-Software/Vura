cmake_minimum_required(VERSION 3.16)

find_package(Qt6 REQUIRED Core)

add_library(libvura STATIC)
add_library(VURA::libvura ALIAS libvura)

target_sources(
        libvura PRIVATE
        libvura.cpp
        libvura.h
)

target_link_libraries(libvura PRIVATE Qt6::Core)
target_include_directories(libvura INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

