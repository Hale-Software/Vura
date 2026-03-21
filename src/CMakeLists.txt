cmake_minimum_required(VERSION 3.28...3.30)

# Define source and destination directories
set(DATA_DIR "${CMAKE_SOURCE_DIR}/src/data")
set(DESTINATION_DIR "${CMAKE_CURRENT_BINARY_DIR}")

# Add a custom target to perform the copy operation
add_custom_target(copy_data_files
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${DATA_DIR}" "${DESTINATION_DIR}/data"
        COMMENT "Copying data files from source to build directory"
)

add_executable(vura)
add_executable(VURA::vura ALIAS vura)
add_dependencies(vura copy_data_files)

target_include_directories(vura PRIVATE "${CMAKE_SOURCE_DIR}/deps" "${CMAKE_SOURCE_DIR}/src/deps" "${CMAKE_SOURCE_DIR}/libvura")

target_link_libraries(
        vura PRIVATE
        xxhash
        VURA::qBreakpad
        VURA::libvura
)

include(cmake/ui-components.cmake)
include(cmake/ui-dialogs.cmake)
include(cmake/ui-models.cmake)
include(cmake/ui-qt.cmake)
include(cmake/ui-settings.cmake)
include(cmake/ui-utility.cmake)
include(cmake/ui-widgets.cmake)

configure_file(cmake/templates/config.h.in config.h)

target_sources(
        vura PRIVATE
        main.cpp
        main.h
        config.h
        constants.cpp
        constants.h
)

if (OS_WINDOWS)
    include(cmake/os-windows.cmake)
elseif (OS_MACOS)
    include(cmake/os-macos.cmake)
elseif (OS_LINUX)
    include(cmake/os-linux.cmake)
endif ()

set_target_properties(vura PROPERTIES
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)

get_target_property(target_sources vura SOURCES)
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

set_target_properties(vura PROPERTIES FOLDER UI OUTPUT_NAME "$<IF:$<PLATFORM_ID:Windows>,vura64,vura>")

include(GNUInstallDirs)
install(TARGETS vura
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(vura)
