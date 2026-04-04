cmake_minimum_required(VERSION 3.28...3.30)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/bootstrap.cmake" NO_POLICY_SCOPE)

project(vura VERSION ${VURA_VERSION_CANONICAL} LANGUAGES CXX C)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/Zc:__cplusplus /permissive-)
endif()


option(ENABLE_TESTS "Enable building with Tests (requires Qt)" OFF)

add_subdirectory(deps)
add_subdirectory(src)

if (ENABLE_TESTS)
    add_subdirectory(tests)
endif ()