cmake_minimum_required(VERSION 3.28...3.30)

project(CrashHandler)
set(VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# configure file
set(APP_DISPLAY_NAME "Vura Crash Handler")
include_directories ("${PROJECT_SOURCE_DIR}/../")

# qt6 required
find_package(Qt6Widgets CONFIG REQUIRED)
find_package(Qt6Network CONFIG REQUIRED)

# sources
file(GLOB SOURCES
        "${PROJECT_SOURCE_DIR}/*.cpp"
        "${PROJECT_SOURCE_DIR}/*.ui"
        "${PROJECT_SOURCE_DIR}/*.qrc"
)

# app information
set(COMPANY "Hale Software")
set(IDENTIFIER "com.hale-software.crashhandler")

# target on APPLE
if(APPLE)
    set(MACOSX_BUNDLE_INFO_STRING "Vura Crash Handler")
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION})
    set(MACOSX_BUNDLE_ICON_FILE "CrashHandler.icns")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.hale-software.crashhandler")
    set(MACOSX_BUNDLE_BUNDLE_NAME "Vura Crash Handler")

    # # icns
    # set(ICON_FILE Resources/CrashHandler.icns)
    # set_source_files_properties(${ICON_FILE}
    #     PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    # target
    add_executable(CrashHandler ${SOURCES} ${ICON_FILE} ${QM_FILES})
endif(APPLE)

# target on WINDOWS
if(WIN32)
    # set(WIN_RC CrashHandler.rc)
    add_executable(CrashHandler ${SOURCES} ${WIN_RC} ${QM_FILES})
endif(WIN32)

# target on LINUX
if(LINUX)
    # set(WIN_RC CrashHandler.rc)
    add_executable(CrashHandler ${SOURCES} ${WIN_RC} ${QM_FILES})
endif(LINUX)

target_link_libraries(CrashHandler Qt6::Widgets Qt6::Network ${FRAMEWORKS})

# configure install
install (TARGETS CrashHandler DESTINATION ${CMAKE_BINARY_DIR}/crash-handler)

# for vs debug
if (WIN32)
    add_custom_command(TARGET CrashHandler POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:CrashHandler> ${PROJECT_BINARY_DIR}/../$(Configuration)
    )
endif(WIN32)
