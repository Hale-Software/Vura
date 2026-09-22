cmake_minimum_required(VERSION 3.28...3.30)

option(MEDIA_ENABLE_MPV "Build the libmpv backend when it is available" OFF)
option(MEDIA_ENABLE_QTMULTIMEDIA "Build the Qt Multimedia backend when available" ON)
option(MEDIA_ENABLE_OPENGL "Build the OpenGL backend when available" OFF)

option(LIBVURA_BUILD_TESTS "Build the tests for libvura" OFF)
option(UI_BUILD_TESTS "Build the tests for UI" OFF)

option(ENABLE_TESTS "Enable building with Tests (requires Qt)" OFF)
option(ENABLE_CPACK "Enable building with CPack" OFF)


include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/bootstrap.cmake" NO_POLICY_SCOPE)

if (ENABLE_CPACK)
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/cpack.cmake" NO_POLICY_SCOPE)
endif ()

project(vura VERSION ${VURA_VERSION_CANONICAL} LANGUAGES CXX C)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/media_engines.cmake" NO_POLICY_SCOPE)

add_subdirectory(libvura)
add_subdirectory(src)

if (ENABLE_TESTS)
    add_subdirectory(tests)
endif ()
