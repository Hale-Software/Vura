cmake_minimum_required(VERSION 3.16)

find_package(Qt6 REQUIRED Core Widgets Network)

add_library(libvura STATIC)
add_library(VURA::libvura ALIAS libvura)

# Root Directory
target_sources(
        libvura
        PUBLIC
        constants.h
        libvura.h
        vura-deserializer.h
        vura-error.h
        vura-overloads.h
        vura-serializer.h
        PRIVATE
        constants.cpp
        libvura.cpp
        vura-deserializer.cpp
        vura-defs.h
        vura-error.cpp
        vura-serializer.cpp
)

# Filetypes Directory
target_sources(
        libvura
        PUBLIC
        filetypes/markers-data.h
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
        util/blog.h
        util/singleinstance.h
        PRIVATE
        util/blog.cpp
        util/singleinstance.cpp
)

target_link_libraries(libvura PRIVATE Qt6::Core Qt6::Widgets Qt6::Network)
target_include_directories(libvura INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

