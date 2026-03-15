# Vura CMake common version helper module

include_guard(GLOBAL)

set(_vura_version ${_vura_default_version})
set(_vura_version_canonical ${_vura_default_version})

# Attempt to automatically discover expected Vura version
if(NOT DEFINED VURA_VERSION_OVERRIDE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    execute_process(
            COMMAND git describe --always --tags --dirty=-modified
            OUTPUT_VARIABLE _vura_version
            ERROR_VARIABLE _git_describe_err
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            RESULT_VARIABLE _vura_version_result
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(_git_describe_err)
        message(FATAL_ERROR "Could not fetch Vura version tag from git.\n" ${_git_describe_err})
    endif()

    if(_vura_version_result EQUAL 0)
        string(REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+).*" "\\1;\\2;\\3" _vura_version_canonical ${_vura_version})
    endif()
elseif(DEFINED VURA_VERSION_OVERRIDE)
    if(VURA_VERSION_OVERRIDE MATCHES "([0-9]+)\\.([0-9]+)\\.([0-9]+).*")
        string(
                REGEX REPLACE
                "([0-9]+)\\.([0-9]+)\\.([0-9]+).*"
                "\\1;\\2;\\3"
                _vura_version_canonical
                ${VURA_VERSION_OVERRIDE}
        )
        set(_vura_version ${VURA_VERSION_OVERRIDE})
    else()
        message(FATAL_ERROR "Invalid version supplied - must be <MAJOR>.<MINOR>.<PATCH>[-(rc|beta)<NUMBER>].")
    endif()
endif()

# Set beta/rc versions if suffix included in version string
if(_vura_version MATCHES "[0-9]+\\.[0-9]+\\.[0-9]+-rc[0-9]+")
    string(REGEX REPLACE "[0-9]+\\.[0-9]+\\.[0-9]+-rc([0-9]+).*$" "\\1" _vura_release_candidate ${_vura_version})
elseif(_vura_version MATCHES "[0-9]+\\.[0-9]+\\.[0-9]+-beta[0-9]+")
    string(REGEX REPLACE "[0-9]+\\.[0-9]+\\.[0-9]+-beta([0-9]+).*$" "\\1" _vura_beta ${_vura_version})
endif()

list(GET _vura_version_canonical 0 VURA_VERSION_MAJOR)
list(GET _vura_version_canonical 1 VURA_VERSION_MINOR)
list(GET _vura_version_canonical 2 VURA_VERSION_PATCH)

set(VURA_RELEASE_CANDIDATE ${_vura_release_candidate})
set(VURA_BETA ${_vura_beta})

string(REPLACE ";" "." VURA_VERSION_CANONICAL "${_vura_version_canonical}")
string(REPLACE ";" "." VURA_VERSION "${_vura_version}")

if(VURA_RELEASE_CANDIDATE GREATER 0)
    message(
            AUTHOR_WARNING
            "******************************************************************************\n"
            "  + Vura - Release candidate detected, VURA_VERSION is now: ${VURA_VERSION}\n"
            "******************************************************************************"
    )
elseif(VURA_BETA GREATER 0)
    message(
            AUTHOR_WARNING
            "******************************************************************************\n"
            "  + Vura - Beta detected, VURA_VERSION is now: ${VURA_VERSION}\n"
            "******************************************************************************"
    )
endif()

unset(_vura_default_version)
unset(_vura_version)
unset(_vura_version_canonical)
unset(_vura_release_candidate)
unset(_vura_beta)
unset(_vura_version_result)