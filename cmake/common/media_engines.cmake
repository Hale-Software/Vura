# Vura CMake media engines module

include_guard(GLOBAL)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Sql Svg)
find_package(Qt6 QUIET COMPONENTS Multimedia MultimediaWidgets OpenGL OpenGLWidgets DBus Test)

# Backend detection

set(VURA_ENABLE_QTMULTIMEDIA FALSE)
if (MEDIA_ENABLE_QTMULTIMEDIA AND Qt6Multimedia_FOUND AND Qt6MultimediaWidgets_FOUND)
    set(VURA_ENABLE_QTMULTIMEDIA TRUE)
endif ()

set(VURA_ENABLE_MPV FALSE)
if (MEDIA_ENABLE_MPV AND Qt6OpenGLWidgets_FOUND)
    find_package(PkgConfig QUIET)
    if (PkgConfig_FOUND)
        pkg_check_modules(MPV IMPORTED_TARGET mpv)
    endif ()
    if (MPV_FOUND)
        set(VURA_ENABLE_MPV TRUE)
    endif ()
endif ()

set(VURA_ENABLE_OPENGL FALSE)
if (MEDIA_ENABLE_OPENGL AND Qt6OpenGLWidgets_FOUND)
    set(VURA_ENABLE_OPENGL TRUE)
endif ()
