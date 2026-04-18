configure_file(cmake/windows/vura.rc.in vura.rc)

target_sources(
        vura
        PRIVATE
        cmake/windows/vura.manifest
        dialogs/updatewindow.cpp
        dialogs/updatewindow.h
        forms/updatewindow.ui
        vura.rc
)

# Set commit for untagged version comparisons in the Windows updater
if(VURA_VERSION MATCHES ".+g[a-f0-9]+.*")
    string(REGEX REPLACE ".+g([a-f0-9]+).*$" "\\1" VURA_COMMIT ${VURA_VERSION})
else()
    set(VURA_COMMIT "")
endif()

set_property(TARGET vura APPEND PROPERTY AUTORCC_OPTIONS --format-version 1)

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT vura)
set_target_properties(
        vura
        PROPERTIES
        WIN32_EXECUTABLE TRUE
        VS_DEBUGGER_COMMAND "${CMAKE_BINARY_DIR}/rundir/$<CONFIG>/bin/64bit/$<TARGET_FILE_NAME:vura>"
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/rundir/$<CONFIG>/bin/64bit"
)
