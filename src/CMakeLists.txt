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

include(cmake/ui-components.cmake)
include(cmake/ui-dialogs.cmake)
#include(cmake/ui-models.cmake)
include(cmake/ui-qt.cmake)
include(cmake/ui-settings.cmake)
include(cmake/ui-updater.cmake)
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

get_target_property(target_sources vura SOURCES)
set(target_cpp_sources ${target_sources})
set(target_hpp_sources ${target_sources})
set(target_qt_sources ${target_sources})
list(FILTER target_cpp_sources INCLUDE REGEX ".+\\.(cpp|mm|c|m)")
list(SORT target_cpp_sources COMPARE NATURAL CASE SENSITIVE ORDER ASCENDING)
list(FILTER target_hpp_sources INCLUDE REGEX ".+\\.(hpp|h)")
list(SORT target_hpp_sources COMPARE NATURAL CASE SENSITIVE ORDER ASCENDING)
list(FILTER target_qt_sources INCLUDE REGEX ".+\\.(ui|qrc)")
list(SORT target_qt_sources COMPARE NATURAL CASE SENSITIVE ORDER ASCENDING)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "Source Files" FILES ${target_cpp_sources})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "Header Files" FILES ${target_hpp_sources})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "Qt Files" FILES ${target_qt_sources})

set_target_properties(vura PROPERTIES FOLDER src OUTPUT_NAME "$<IF:$<PLATFORM_ID:Windows>,vura64,vura>")

install(TARGETS vura
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(WIN32)
    qt_generate_deploy_app_script(
            TARGET vura
            OUTPUT_SCRIPT deploy_script
            NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})
endif()

# ==============================================================================
# 2. CPack Global Configuration
# ==============================================================================
set(CPACK_PACKAGE_NAME "${VURA_PRODUCT_NAME}")
set(CPACK_PACKAGE_VENDOR "${VURA_COMPANY_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${VURA_COMMENTS}")
set(CPACK_PACKAGE_VERSION_MAJOR ${VURA_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VURA_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VURA_VERSION_PATCH})
set(CPACK_PACKAGE_CONTACT "${VURA_WEBSITE}")

# ==============================================================================
# 3. Platform-Specific Installer Configurations
# ==============================================================================
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    # WINDOWS: Generate an NSIS Setup Wizard (.exe)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_MUI_ICON "${CMAKE_BINARY_DIR}/src/assets/icons/vura.ico")
    set(CPACK_NSIS_INSTDIR_REGKEY "Software\\\\Vura")
    set(CPACK_NSIS_DISPLAY_NAME "Vura")
    set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Vura.lnk\\\" \\\"$INSTDIR\\\\bin\\\\vura64.exe\\\"")

elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    # macOS: Generate a Drag-and-Drop Disk Image (.dmg)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_FORMAT "UDBZ")
    set(CPACK_DMG_VOLUME_NAME "Vura")
    # Optional: Add a custom background image showing the "Drag to Applications" arrow
    # set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_CURRENT_SOURCE_DIR}/assets/dmg_bg.png")

elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    # LINUX: Generate both Debian (.deb) and RPM (.rpm) packages
    set(CPACK_GENERATOR "DEB;RPM")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR} <${CPACK_PACKAGE_CONTACT}>")

    # Define dependencies so the OS package manager automatically installs them
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6core6a, libqt6gui6, libavcodec, libavformat, libavutil, libswscale")
    set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qtbase, ffmpeg-libs")

    # Standard installation directory for Linux
    set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/vura")
endif()

# ==============================================================================
# 4. Include CPack (CRITICAL: MUST BE THE LAST LINE)
# ==============================================================================
include(CPack)
