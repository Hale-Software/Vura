cmake_minimum_required(VERSION 3.28...3.30)

project(VuraBreakpad)
set(VERSION 1.0.0)
ADD_DEFINITIONS(-DUNICODE)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# configure file
set(APP_DISPLAY_NAME "Vura Breakpad")
configure_file (
        "${PROJECT_SOURCE_DIR}/app/app_version.h.in"
        "${PROJECT_SOURCE_DIR}/app/app_version.h"
)

# install prefix
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/install")

# qt6 required
find_package(Qt6Widgets CONFIG REQUIRED)
find_package(Qt6Network CONFIG REQUIRED)

############################################################
# build crash Handler
add_subdirectory(crashhandler)

# build external breakpad repo
set(BREAKPAD_SOURCE_DIR "${PROJECT_SOURCE_DIR}/external/breakpad")
include_directories ("${BREAKPAD_SOURCE_DIR}/src")
add_subdirectory (external)

############################################################
# breakpad handler
include_directories ("${PROJECT_SOURCE_DIR}/breakpad/")

# sources
file(GLOB SOURCES
        "${PROJECT_SOURCE_DIR}/breakpad/*.cpp"
)

############################################################
# translate
# file(GLOB TS_FILES
#     "./*.ts"
# )
# qt6_create_translation(QM_FILES RawPlus ${SOURCES} ${TS_FILES})
# if(APPLE)
#     set_source_files_properties(
#         ${QM_FILES}
#         PROPERTIES
#         MACOSX_PACKAGE_LOCATION "MacOS"
#     )
# endif(APPLE)

############################################################
# app information
set(COMPANY "Hale Software")
set(IDENTIFIER "com.hale-software.vurabreakpad")

# target on APPLE
if(APPLE)
    set(MACOSX_BUNDLE_INFO_STRING "Vura Breakpad")
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION})
    set(MACOSX_BUNDLE_ICON_FILE "VuraBreakpad.icns")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.hale-software.vurabreakpad")
    set(MACOSX_BUNDLE_BUNDLE_NAME "Vura Breakpad")

    # # icns
    # set(ICON_FILE Resources/Vura Breakpad.icns)
    # set_source_files_properties(${ICON_FILE}
    #     PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    # framework
    find_library(corefoundation_lib CoreFoundation)
    find_library(cfnetwork_lib CFNetwork)
    set(FRAMEWORKS
            ${cfnetwork_lib}
            ${corefoundation_lib})

    # target
    add_executable(VuraBreakpad ${SOURCES} ${BREAKPAD_SRCS} ${ICON_FILE} ${QM_FILES})
endif(APPLE)

# target on WINDOWS
if(WIN32)
    # set(WIN_RC VuraBreakpad.rc)
    add_executable(VuraBreakpad ${SOURCES} ${BREAKPAD_SRCS} ${WIN_RC} ${QM_FILES})
endif(WIN32)

# target on LINUX
if(LINUX)
    # set(WIN_RC VuraBreakpad.rc)
    add_executable(VuraBreakpad ${SOURCES} ${BREAKPAD_SRCS} ${WIN_RC} ${QM_FILES})
endif(LINUX)

# linking
target_link_libraries(VuraBreakpad Qt6::Widgets Qt6::Network ${FRAMEWORKS} libbreakpad)

# configure install
install (TARGETS VuraBreakpad DESTINATION ${CMAKE_BINARY_DIR}/crash-handler)
