# Vura CMake bootstrap module

include_guard(GLOBAL)

# Map fallback configurations for optimized build configurations
# gersemi: off
set(
        CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO
        RelWithDebInfo
        Release
        MinSizeRel
        None
        ""
)
set(
        CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL
        MinSizeRel
        Release
        RelWithDebInfo
        None
        ""
)
set(
        CMAKE_MAP_IMPORTED_CONFIG_RELEASE
        Release
        RelWithDebInfo
        MinSizeRel
        None
        ""
)
# gersemi: on

# Prohibit in-source builds
if("${CMAKE_CURRENT_BINARY_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    message(
            FATAL_ERROR
            "In-source builds of Vura are not supported. "
            "Specify a build directory via 'cmake -S <SOURCE DIRECTORY> -B <BUILD_DIRECTORY>' instead."
    )
    file(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/CMakeCache.txt" "${CMAKE_CURRENT_SOURCE_DIR}/CMakeFiles")
endif()

# Set default global project variables
set(VURA_COMPANY_NAME "Hale Software")
set(VURA_PRODUCT_NAME "Vura")
set(VURA_WEBSITE "https://github.com/Hale-Software/Vura")
set(VURA_COMMENTS "Free and open source video player and editor designed for customization.")
set(VURA_LEGAL_COPYRIGHT "(C) Hale Software")
set(VURA_CMAKE_VERSION 3.0.0)

# Configure default version strings
set(_vura_default_version "0" "0" "1")
set(_vura_release_candidate 0)
set(_vura_beta 0)

# Add common module directories to default search path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/common")

include(versionconfig)
include(buildnumber)
include(osconfig)

# Enable default inclusion of targets' source and binary directory
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)