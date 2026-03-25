if(NOT TARGET VURA::obfuscate)
    add_library(vura-obfuscate INTERFACE)
    add_library(VURA::obfuscate ALIAS vura-obfuscate)
    target_sources(vura-obfuscate INTERFACE util/windows/obfuscate.c util/windows/obfuscate.h)
    target_include_directories(vura-obfuscate INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

if(NOT TARGET VURA::comutils)
    add_library(vura-comutils INTERFACE)
    add_library(VURA::COMutils ALIAS vura-comutils)
    target_sources(vura-comutils INTERFACE util/windows/ComPtr.hpp)
    target_include_directories(vura-comutils INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

if(NOT TARGET VURA::winhandle)
    add_library(vura-winhandle INTERFACE)
    add_library(VURA::winhandle ALIAS vura-winhandle)
    target_sources(vura-winhandle INTERFACE util/windows/WinHandle.hpp)
    target_include_directories(vura-winhandle INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

if(NOT TARGET VURA::threading-windows)
    add_library(vura-threading-windows INTERFACE)
    add_library(VURA::threading-windows ALIAS vura-threading-windows)
    target_sources(vura-threading-windows INTERFACE util/threading-windows.h)
    target_include_directories(vura-threading-windows INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

if(NOT TARGET VURA::w32-pthreads)
    add_subdirectory("${CMAKE_SOURCE_DIR}/deps/w32-pthreads" "${CMAKE_BINARY_DIR}/deps/w32-pthreads")
endif()

if(NOT VURA_PARENT_ARCHITECTURE STREQUAL CMAKE_VS_PLATFORM_NAME)
    return()
endif()

configure_file(cmake/windows/vura-module.rc.in libvura.rc)

target_sources(
        libvura
        PRIVATE
        libvura.rc
        vura-win-crash-handler.c
        vura-windows.c
        util/pipe-windows.c
        util/platform-windows.c
        util/threading-windows.c
        util/threading-windows.h
        util/windows/CoTaskMemPtr.hpp
        util/windows/HRError.hpp
        util/windows/win-registry.h
        util/windows/win-version.h
        util/windows/window-helpers.c
        util/windows/window-helpers.h
)

target_compile_options(libvura PRIVATE $<$<COMPILE_LANGUAGE:C,CXX>:/EHc->)

set_source_files_properties(
        vura-win-crash-handler.c
        PROPERTIES COMPILE_DEFINITIONS VURA_VERSION="${VURA_VERSION_CANONICAL}"
)

target_link_libraries(
        libvura
        PRIVATE Avrt Dwmapi Dxgi winmm Rpcrt4 VURA::obfuscate VURA::winhandle VURA::COMutils
        PUBLIC VURA::w32-pthreads
)

target_link_options(libvura PRIVATE /IGNORE:4098 /SAFESEH:NO)

set_target_properties(libvura PROPERTIES PREFIX "" OUTPUT_NAME "vura")
