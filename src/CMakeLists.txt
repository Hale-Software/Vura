cmake_minimum_required(VERSION 3.28...3.30)

# Define application assets source and destination directories
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
set(DESTINATION_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(FFMPEG_DIR "C:\\Users\\halea\\bin\\ffmpeg-8.1.1-full_build-shared")

# Add a custom target to perform the copy operation
add_custom_target(copy_asset_files
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${ASSETS_DIR}" "${DESTINATION_DIR}/assets"
        COMMENT "Copying application asset files from source to build directory"
)

add_executable(vura)
add_executable(VURA::vura ALIAS vura)
add_dependencies(vura copy_asset_files)

add_custom_command(TARGET vura POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/AUTHORS"
        "$<TARGET_FILE_DIR:vura>/assets/data/AUTHORS"
        COMMENT "Copying authors file to build directory"
)

target_link_libraries(
    vura PRIVATE
    VURA::libvura
    ${FFMPEG_DIR}/lib/avformat.lib
    ${FFMPEG_DIR}/lib/avcodec.lib
    ${FFMPEG_DIR}/lib/avutil.lib
    ${FFMPEG_DIR}/lib/swscale.lib
    ${FFMPEG_DIR}/lib/swresample.lib
)

target_include_directories(
    vura PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/components
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/forms
    ${CMAKE_CURRENT_SOURCE_DIR}/models
    ${CMAKE_CURRENT_SOURCE_DIR}/settings
    ${CMAKE_CURRENT_SOURCE_DIR}/updater
    ${CMAKE_CURRENT_SOURCE_DIR}/utility
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets
    ${FFMPEG_DIR}/include
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# libvura's Q_OBJECT classes are already moc'd inside liblibvura.a. When a vura
# source #includes one of those headers, this target's AUTOMOC would re-generate
# the same moc code, producing duplicate symbols that clash with the library's
# at link time ("multiple definition of PlaylistModel::staticMetaObject", ...).
# Skip AUTOMOC for all libvura headers here so they are moc'd only in libvura.
file(GLOB_RECURSE _libvura_headers CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/libvura/libvura/*.h")
set_source_files_properties(${_libvura_headers} PROPERTIES SKIP_AUTOMOC ON)

include(cmake/ui-components.cmake)
include(cmake/ui-dialogs.cmake)
#include(cmake/ui-models.cmake)
include(cmake/ui-qt.cmake)
include(cmake/ui-settings.cmake)
#include(cmake/ui-updater.cmake)
include(cmake/ui-utility.cmake)
include(cmake/ui-widgets.cmake)

configure_file(cmake/templates/ui-config.h.in ui-config.h)

target_sources(
        vura PRIVATE
        main.cpp
        ui-config.h
)

if (OS_WINDOWS)
    include(cmake/os-windows.cmake)
elseif (OS_MACOS)
    include(cmake/os-macos.cmake)
elseif (OS_LINUX)
    include(cmake/os-linux.cmake)
endif ()

set_target_properties(vura PROPERTIES
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)

#include(helpers)
#vura_installer()
include(cpack_windows)
vura_deploy(vura)
