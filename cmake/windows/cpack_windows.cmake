include_guard(GLOBAL)

function(vura_deployer vura_target)
    get_target_property(target_sources ${vura_target} SOURCES)
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

    set_target_properties(${vura_target} PROPERTIES FOLDER src OUTPUT_NAME "$<IF:$<PLATFORM_ID:Windows>,vura64,vura>")

    install(TARGETS ${vura_target}
            BUNDLE DESTINATION .
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    if(WIN32)
        qt_generate_deploy_app_script(
                TARGET ${vura_target}
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
endfunction()

function(vura_deploy vura_target)
    include(TargetArch)
    target_architecture(TARGET_ARCH)
    get_target_property(target_sources ${vura_target} SOURCES)
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

    set_target_properties(${vura_target} PROPERTIES FOLDER src OUTPUT_NAME "$<IF:$<PLATFORM_ID:Windows>,vura64,vura>")

    install(TARGETS ${vura_target}
            BUNDLE DESTINATION .
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    if(WIN32)
        qt_generate_deploy_app_script(
                TARGET ${vura_target}
                OUTPUT_SCRIPT deploy_script
                NO_UNSUPPORTED_PLATFORM_ERROR
        )
        install(SCRIPT ${deploy_script})
    endif()

    set(CPACK_PACKAGE_NAME "${VURA_PRODUCT_NAME}")
    set(CPACK_PACKAGE_VENDOR "${VURA_COMPANY_NAME}")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${VURA_COMMENTS}")
    set(CPACK_PACKAGE_VERSION_MAJOR ${VURA_VERSION_MAJOR})
    set(CPACK_PACKAGE_VERSION_MINOR ${VURA_VERSION_MINOR})
    set(CPACK_PACKAGE_VERSION_PATCH ${VURA_VERSION_PATCH})
    set(CPACK_PACKAGE_CONTACT "${VURA_WEBSITE}")
    set(CPACK_PACKAGE_FILE_NAME "vura-${VURA_VERSION_CANONICAL}-windows-${TARGET_ARCH}")
    set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY FALSE)
    set(CPACK_GENERATOR ZIP)

    include(CPack)
endfunction()
