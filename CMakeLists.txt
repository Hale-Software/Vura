cmake_minimum_required(VERSION 3.28...3.30)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/bootstrap.cmake" NO_POLICY_SCOPE)

project(vura VERSION ${VURA_VERSION_CANONICAL} LANGUAGES CXX C)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_UI "Enable building with UI frontend (requires Qt6)" ON)
option(ENABLE_TESTS "Enable building tests" OFF)

add_subdirectory(deps)

if (ENABLE_UI)
    add_subdirectory(src)
endif ()

add_subdirectory(tests)
