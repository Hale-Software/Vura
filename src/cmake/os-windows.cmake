configure_file(cmake/windows/vura.rc.in vura.rc)

target_sources(
        vura
        PRIVATE
        cmake/windows/vura.manifest
        dialogs/UpdateChecker.cpp
        dialogs/UpdateChecker.h
        vura.rc
)

set_target_properties(
    vura PROPERTIES
    WIN32_EXECUTABLE TRUE
)


# Set commit for untagged version comparisons in the Windows updater
#if(VURA_VERSION MATCHES ".+g[a-f0-9]+.*")
#    string(REGEX REPLACE ".+g([a-f0-9]+).*$" "\\1" VURA_COMMIT ${VURA_VERSION})
#else()
#    set(VURA_COMMIT "")
#endif()

#set_property(TARGET vura APPEND PROPERTY AUTORCC_OPTIONS --format-version 1)
