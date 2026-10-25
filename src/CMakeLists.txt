cmake_minimum_required(VERSION 3.28...3.30)

# Define application assets source and destination directories
#set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
#set(DESTINATION_DIR "${CMAKE_CURRENT_BINARY_DIR}")

# Add a custom target to perform the copy operation
#add_custom_target(copy_asset_files
#        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${ASSETS_DIR}" "${DESTINATION_DIR}/assets"
#        COMMENT "Copying application asset files from source to build directory"
#)

add_executable(vura)
add_executable(VURA::vura ALIAS vura)
#add_dependencies(vura copy_asset_files)

include(cmake/utilities.cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(cmake/ui-components.cmake)
include(cmake/ui-dialogs.cmake)
include(cmake/ui-qt.cmake)
include(cmake/ui-settings.cmake)
include(cmake/ui-utility.cmake)
include(cmake/ui-widgets.cmake)

configure_file(cmake/templates/ui-config.h.in ui-config.h)

target_sources(
    vura PRIVATE
    main.cpp
    vura.cpp
    vura.h
    ui-config.h
)

if (OS_WINDOWS)
    include(cmake/os-windows.cmake)
elseif (OS_MACOS)
    include(cmake/os-macos.cmake)
elseif (OS_LINUX)
    include(cmake/os-linux.cmake)
endif ()

#install(TARGETS vura RUNTIME DESTINATION bin)
install(TARGETS vura
	BUNDLE DESTINATION .
	RUNTIME DESTINATION bin
)
