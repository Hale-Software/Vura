cmake_minimum_required(VERSION 3.28...3.30)

#add_subdirectory("${CMAKE_SOURCE_DIR}/libvura" libvura)

# Define application assets source and destination directories
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
set(DESTINATION_DIR "${CMAKE_CURRENT_BINARY_DIR}")

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

target_link_libraries(vura PRIVATE VURA::libvura)

include(cmake/ui-components.cmake)
include(cmake/ui-dialogs.cmake)
include(cmake/ui-models.cmake)
include(cmake/ui-qt.cmake)
include(cmake/ui-settings.cmake)
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
        COMPONENT Core
)

if(WIN32)
    qt_generate_deploy_app_script(
            TARGET vura
            OUTPUT_SCRIPT deploy_script
            NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})
endif()

install(DIRECTORY "${CMAKE_SOURCE_DIR}/assets/"
        DESTINATION "${CMAKE_INSTALL_BINDIR}/assets"
)

set(CPACK_PACKAGE_NAME "Vura")
set(CPACK_PACKAGE_VENDOR "${VURA_COMPANY_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${VURA_COMMENTS}")
set(CPACK_PACKAGE_HOMEPAGE_URL "${VURA_WEBSITE}")

set(CPACK_PACKAGE_VERSION_MAJOR "${VURA_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VURA_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VURA_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION "${VURA_VERSION_CANONICAL}")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/assets/data/license.txt")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CPACK_GENERATOR "NSIS")

    set(CPACK_NSIS_DISPLAY_NAME "Vura")
    set(CPACK_NSIS_PACKAGE_NAME "Vura")
    set(CPACK_NSIS_CONTACT "https://github.com/Hale-Software/Vura")
    set(CPACK_NSIS_URL_INFO_ABOUT "${VURA_WEBSITE}")
    set(CPACK_NSIS_HELP_LINK "${VURA_WEBSITE}")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

    set(CPACK_PACKAGE_EXECUTABLES "vura64" "Vura")
    set(CPACK_CREATE_DESKTOP_LINKS "Vura")
    set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/cmake/windows/vura.ico")
    set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/cmake/windows/vura.ico")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "Vura")

    install(CODE " " COMPONENT assoc_mp4)
    install(CODE " " COMPONENT assoc_mkv)
    install(CODE " " COMPONENT assoc_mp3)

    include(CPack)
    include(CPackComponent)

    # Force CPack to package the core app AND the association checkboxes
    set(CPACK_COMPONENTS_ALL Core assoc_mp4 assoc_mkv assoc_mp3)

    # Create an empty physical file in your build directory
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/assoc_dummy.tmp" "")

    # Install this empty file into the components so CPack thinks they are "real"
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/assoc_dummy.tmp" DESTINATION bin COMPONENT assoc_mp4)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/assoc_dummy.tmp" DESTINATION bin COMPONENT assoc_mkv)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/assoc_dummy.tmp" DESTINATION bin COMPONENT assoc_mp3)

    cpack_add_component(Core DISPLAY_NAME "Vura Core Files" REQUIRED)

    cpack_add_component_group(Associations DISPLAY_NAME "File Associations" EXPANDED)
    cpack_add_component_group(Video DISPLAY_NAME "Video Files" PARENT_GROUP Associations EXPANDED)
    cpack_add_component_group(Audio DISPLAY_NAME "Audio Files" PARENT_GROUP Associations EXPANDED)

    cpack_add_component(assoc_mp4 DISPLAY_NAME ".mp4 File" GROUP Video)
    cpack_add_component(assoc_mkv DISPLAY_NAME ".mkv File" GROUP Video)
    cpack_add_component(assoc_mp3 DISPLAY_NAME ".mp3 File" GROUP Audio)

    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS [=[
        !include LogicLib.nsh
        !include Sections.nsh

        ; --- Video Formats ---
        ${If} ${SectionIsSelected} ${assoc_mp4}
            WriteRegStr HKCR ".mp4" "" "Vura.Video"
        ${EndIf}

        ${If} ${SectionIsSelected} ${assoc_mkv}
            WriteRegStr HKCR ".mkv" "" "Vura.Video"
        ${EndIf}

        ; Base Video Registration (Registers if ANY video format is checked)
        ${If} ${SectionIsSelected} ${assoc_mp4}
        ${OrIf} ${SectionIsSelected} ${assoc_mkv}
            WriteRegStr HKCR "Vura.Video" "" "Vura Video File"
            WriteRegStr HKCR "Vura.Video\DefaultIcon" "" "$INSTDIR\bin\vura64.exe,0"
            WriteRegStr HKCR "Vura.Video\shell\open\command" "" '"$INSTDIR\bin\vura64.exe" "%1"'
        ${EndIf}

        ; --- Audio Formats ---
        ${If} ${SectionIsSelected} ${assoc_mp3}
            WriteRegStr HKCR ".mp3" "" "Vura.Audio"

            WriteRegStr HKCR "Vura.Audio" "" "Vura Audio File"
            WriteRegStr HKCR "Vura.Audio\DefaultIcon" "" "$INSTDIR\bin\vura64.exe,0"
            WriteRegStr HKCR "Vura.Audio\shell\open\command" "" '"$INSTDIR\bin\vura64.exe" "%1"'
        ${EndIf}

        ; Refresh Windows Explorer icons instantly
        System::Call 'shell32.dll::SHChangeNotify(i 0x08000000, i 0, i 0, i 0)'

        ; Delete the dummy file used to trick CMake into showing the UI checkboxes
        Delete "$INSTDIR\bin\assoc_dummy.tmp"
    ]=])

    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS [=[
        DeleteRegKey HKCR ".mp4"
        DeleteRegKey HKCR ".mkv"
        DeleteRegKey HKCR ".mp3"
        DeleteRegKey HKCR "Vura.Video"
        DeleteRegKey HKCR "Vura.Audio"

        System::Call 'shell32.dll::SHChangeNotify(i 0x08000000, i 0, i 0, i 0)'
    ]=])

    # Create Desktop Shortcut
    set(CPACK_NSIS_CREATE_ICONS_EXTRA "
        CreateShortCut '$DESKTOP\\\\Vura.lnk' '$INSTDIR\\\\bin\\\\vura64.exe'
    ")

    # Delete Desktop Shortcut on Uninstall
    set(CPACK_NSIS_DELETE_ICONS_EXTRA "
        Delete '$DESKTOP\\\\Vura.lnk'
    ")

else()
    set(CPACK_GENERATOR "ZIP")
    include(CPack)
endif()
