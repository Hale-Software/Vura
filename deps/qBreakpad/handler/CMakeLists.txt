cmake_minimum_required(VERSION 3.16)
project(qBreakpad VERSION 0.4.0)

# Ensure Qt handles Meta-Object Compiler (MOC) automatically
set(CMAKE_AUTOMOC ON)

# 1. Find Dependencies
find_package(Qt6 REQUIRED COMPONENTS Core Network)

# 2. Define the Library
# Using STATIC here as it's common for breakpad wrappers
add_library(qBreakpad STATIC)
add_library(VURA::qBreakpad ALIAS qBreakpad)

# 3. Add Sources and Headers
target_sources(qBreakpad PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/QBreakpadHandler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/QBreakpadHttpUploader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/singletone/call_once.h
        ${CMAKE_CURRENT_SOURCE_DIR}/singletone/singleton.h
        ${CMAKE_CURRENT_SOURCE_DIR}/QBreakpadHandler.h
        ${CMAKE_CURRENT_SOURCE_DIR}/QBreakpadHttpUploader.h
)

# 4. Include Directories (replaces INCLUDEPATH)
target_include_directories(qBreakpad PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/singletone
)

# 5. Link Qt Libraries
target_link_libraries(qBreakpad PRIVATE
        Qt6::Core
        Qt6::Network
)

# 6. Replicating QMake's Sub-Includes
# Note: In CMake, it's better if these 'third_party' folders have their own CMakeLists.txt.
# If they are just lists of files, use 'include()'.
include("${CMAKE_CURRENT_SOURCE_DIR}/../config.cmake" OPTIONAL)
include("${CMAKE_CURRENT_SOURCE_DIR}/../third_party/breakpad.cmake" OPTIONAL)

# 7. Translation of CONFIG += warn_on thread exceptions rtti stl
# Most of these are ON by default in modern CMake, but to be explicit:
set_target_properties(qBreakpad PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

# 8. Handling Output Directories (OBJECTS_DIR / DESTDIR)
set_target_properties(qBreakpad PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        FOLDER deps
)
