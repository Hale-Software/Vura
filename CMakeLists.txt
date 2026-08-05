cmake_minimum_required(VERSION 3.28...3.30)

option(ENABLE_TESTS "Enable building with Tests (requires Qt)" OFF)
option(ENABLE_CPACK "Enable building with CPack" OFF)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/bootstrap.cmake" NO_POLICY_SCOPE)

if (ENABLE_CPACK)
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/cpack.cmake" NO_POLICY_SCOPE)
endif ()

project(vura VERSION ${VURA_VERSION_CANONICAL} LANGUAGES CXX C)

add_subdirectory(libvura)
add_subdirectory(src)

if (ENABLE_TESTS)
    add_subdirectory(tests)
endif ()
