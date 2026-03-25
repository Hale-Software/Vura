cmake_minimum_required(VERSION 3.28...3.30)

find_package(Qt6 COMPONENTS Widgets Network)

include(cmake/vura-version.cmake)

#if(OS_WINDOWS AND NOT VURA_PARENT_ARCHITECTURE STREQUAL CMAKE_VS_PLATFORM_NAME)
#    include(cmake/os-windows.cmake)
#    return()
#endif()

#find_package(SIMDe REQUIRED)
#find_package(Threads REQUIRED)

#find_package(FFmpeg 6.1 REQUIRED avformat avutil swscale swresample OPTIONAL_COMPONENTS avcodec)

#find_package(ZLIB REQUIRED)
#find_package(Uthash REQUIRED)

#find_package(jansson REQUIRED)
#if(NOT TARGET VURA::caption)
#    add_subdirectory("${CMAKE_SOURCE_DIR}/deps/libcaption" "${CMAKE_BINARY_DIR}/deps/libcaption")
#endif()

add_library(libvura SHARED)
add_library(VURA::libvura ALIAS libvura)

target_sources(
        libvura
        PRIVATE
#        $<$<BOOL:${ENABLE_HEVC}>:vura-hevc.c>
#        $<$<BOOL:${ENABLE_HEVC}>:vura-hevc.h>
#        vura-audio-controls.c
#        vura-audio-controls.h
#        vura-audio.c
#        vura-av1.c
#        vura-av1.h
#        vura-avc.c
#        vura-avc.h
#        vura-canvas.c
#        vura-config.h
#        vura-data.c
#        vura-data.h
#        vura-defs.h
#        vura-display.c
#        vura-encoder.c
#        vura-encoder.h
#        vura-ffmpeg-compat.h
#        vura-hotkey-name-map.c
#        vura-hotkey.c
#        vura-hotkey.h
#        vura-hotkeys.h
#        vura-interaction.h
#        vura-internal.h
#        vura-missing-files.c
#        vura-missing-files.h
#        vura-module.c
#        vura-module.h
#        vura-nal.c
#        vura-nal.h
#        vura-output-delay.c
#        vura-output.c
#        vura-output.h
#        vura-properties.c
#        vura-properties.h
#        vura-scene.c
#        vura-scene.h
#        vura-service.c
#        vura-service.h
#        vura-source-deinterlace.c
#        vura-source-transition.c
#        vura-source.c
#        vura-source.h
#        vura-video-gpu-encode.c
#        vura-video.c
#        vura-view.c
        vura.c
        vura.h
#        vura.hpp
)

target_sources(
        libvura
        PRIVATE
#        util/array-serializer.c
#        util/array-serializer.h
        util/base.c
        util/base.h
#        util/bitstream.c
#        util/bitstream.h
#        util/bmem.c
#        util/bmem.h
#        util/buffered-file-serializer.c
#        util/buffered-file-serializer.h
        util/c99defs.h
#        util/cf-lexer.c
#        util/cf-lexer.h
#        util/cf-parser.c
#        util/cf-parser.h
#        util/config-file.c
#        util/config-file.h
#        util/constants.c
#        util/constants.h
#        util/crc32.c
#        util/crc32.h
#        util/curl/curl-helper.h
#        util/darray.h
#        util/deque.h
#        util/dstr.c
#        util/dstr.h
#        util/file-serializer.c
#        util/file-serializer.h
#        util/lexer.c
#        util/lexer.h
#        util/pipe.c
#        util/pipe.h
        util/platform.c
        util/platform.h
#        util/profiler.c
#        util/profiler.h
#        util/profiler.hpp
#        util/serializer.h
#        util/source-profiler.c
#        util/source-profiler.h
#        util/sse-intrin.h
#        util/task.c
#        util/task.h
#        util/text-lookup.c
#        util/text-lookup.h
#        util/threading.h
#        util/utf8.c
#        util/utf8.h
#        util/uthash.h
#        util/util.hpp
#        util/util_uint128.h
#        util/util_uint64.h
)

#target_sources(
#        libvura
#        PRIVATE
#        callback/calldata.c
#        callback/calldata.h
#        callback/decl.c
#        callback/decl.h
#        callback/proc.c
#        callback/proc.h
#        callback/signal.c
#        callback/signal.h
#)

#target_sources(
#        libvura
#        PRIVATE
#        media-io/audio-io.c
#        media-io/audio-io.h
#        media-io/audio-math.h
#        media-io/audio-resampler-ffmpeg.c
#        media-io/audio-resampler.h
#        media-io/format-conversion.c
#        media-io/format-conversion.h
#        media-io/frame-rate.h
#        media-io/media-io-defs.h
#        media-io/media-remux.c
#        media-io/media-remux.h
#        media-io/video-fourcc.c
#        media-io/video-frame.c
#        media-io/video-frame.h
#        media-io/video-io.c
#        media-io/video-io.h
#        media-io/video-matrices.c
#        media-io/video-scaler-ffmpeg.c
#        media-io/video-scaler.h
#)

#target_sources(
#        libvura
#        PRIVATE
#        graphics/axisang.c
#        graphics/axisang.h
#        graphics/bounds.c
#        graphics/bounds.h
#        graphics/device-exports.h
#        graphics/effect-parser.c
#        graphics/effect-parser.h
#        graphics/effect.c
#        graphics/effect.h
#        graphics/graphics-ffmpeg.c
#        graphics/graphics-imports.c
#        graphics/graphics-internal.h
#        graphics/graphics.c
#        graphics/graphics.h
#        graphics/half.h
#        graphics/image-file.c
#        graphics/image-file.h
#        graphics/input.h
#        graphics/libnsgif/libnsgif.c
#        graphics/libnsgif/libnsgif.h
#        graphics/math-defs.h
#        graphics/math-extra.c
#        graphics/math-extra.h
#        graphics/matrix3.c
#        graphics/matrix3.h
#        graphics/matrix4.c
#        graphics/matrix4.h
#        graphics/plane.c
#        graphics/plane.h
#        graphics/quat.c
#        graphics/quat.h
#        graphics/shader-parser.c
#        graphics/shader-parser.h
#        graphics/srgb.h
#        graphics/texture-render.c
#        graphics/vec2.c
#        graphics/vec2.h
#        graphics/vec3.c
#        graphics/vec3.h
#        graphics/vec4.c
#        graphics/vec4.h
#)

# Temporarily disables deprecation warnings while vura_data_autoselect_* is deprecated.
if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    set_source_files_properties(vura-data.c PROPERTIES COMPILE_OPTIONS "/wd4996")
elseif(
        CMAKE_C_COMPILER_ID STREQUAL "GNU"
        OR CMAKE_C_COMPILER_ID STREQUAL "Clang"
        OR CMAKE_C_COMPILER_ID STREQUAL "AppleClang"
)
    set_source_files_properties(vura-data.c PROPERTIES COMPILE_OPTIONS "-Wno-deprecated-declarations")
endif()

target_compile_features(libvura PUBLIC cxx_std_17)

target_compile_definitions(
        libvura
        PRIVATE IS_LIBVURA
        PUBLIC
        $<BUILD_INTERFACE:$<$<BOOL:${ENABLE_HEVC}>:ENABLE_HEVC>>
        $<BUILD_INTERFACE:$<$<BOOL:${ENABLE_FFMPEG_MUX_DEBUG}>:SHOW_SUBPROCESSES>>
)

target_link_libraries(
        libvura
        PRIVATE
        Qt6::Widgets
        Qt6::Network
#        VURA::caption
        VURA::libvura-version
#        FFmpeg::avcodec
#        FFmpeg::avformat
#        FFmpeg::avutil
#        FFmpeg::swscale
#        FFmpeg::swresample
#        jansson::jansson
#        Uthash::Uthash
#        ZLIB::ZLIB
#        PUBLIC SIMDe::SIMDe Threads::Threads
)

if(OS_WINDOWS)
    include(cmake/os-windows.cmake)
elseif(OS_MACOS)
    include(cmake/os-macos.cmake)
elseif(OS_LINUX)
    include(cmake/os-linux.cmake)
elseif(OS_FREEBSD OR OS_OPENBSD)
    include(cmake/os-freebsd.cmake)
endif()

configure_file(vuraconfig.h.in "${CMAKE_BINARY_DIR}/config/vuraconfig.h" @ONLY)

target_include_directories(
        libvura
        PUBLIC "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/config>" "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)

set(
        public_headers
#        callback/calldata.h
#        callback/decl.h
#        callback/proc.h
#        callback/signal.h
#        graphics/axisang.h
#        graphics/bounds.h
#        graphics/effect-parser.h
#        graphics/effect.h
#        graphics/graphics.h
#        graphics/image-file.h
#        graphics/input.h
#        graphics/libnsgif/libnsgif.h
#        graphics/math-defs.h
#        graphics/math-extra.h
#        graphics/matrix3.h
#        graphics/matrix4.h
#        graphics/plane.h
#        graphics/quat.h
#        graphics/shader-parser.h
#        graphics/srgb.h
#        graphics/vec2.h
#        graphics/vec3.h
#        graphics/vec4.h
#        media-io/audio-io.h
#        media-io/audio-math.h
#        media-io/audio-resampler.h
#        media-io/format-conversion.h
#        media-io/frame-rate.h
#        media-io/media-io-defs.h
#        media-io/media-remux.h
#        media-io/video-frame.h
#        media-io/video-io.h
#        media-io/video-scaler.h
#        vura-audio-controls.h
#        vura-avc.h
#        vura-config.h
#        vura-data.h
#        vura-defs.h
#        vura-encoder.h
#        vura-hotkey.h
#        vura-hotkeys.h
#        vura-interaction.h
#        vura-missing-files.h
#        vura-module.h
#        vura-nal.h
#        vura-nix-platform.h
#        vura-output.h
#        vura-properties.h
#        vura-service.h
#        vura-source.h
        vura.h
        vura.hpp
#        util/array-serializer.h
        util/base.h
#        util/bitstream.h
#        util/bmem.h
        util/c99defs.h
#        util/cf-lexer.h
#        util/cf-parser.h
#        util/config-file.h
#        util/crc32.h
#        util/darray.h
#        util/deque.h
#        util/dstr.h
#        util/dstr.hpp
#        util/file-serializer.h
#        util/lexer.h
#        util/pipe.h
#        util/platform.h
#        util/profiler.h
#        util/profiler.hpp
#        util/serializer.h
#        util/sse-intrin.h
#        util/task.h
#        util/text-lookup.h
#        util/threading-posix.h
#        util/threading.h
#        util/uthash.h
#        util/util.hpp
#        util/util_uint128.h
#        util/util_uint64.h
)

if(OS_WINDOWS)
    list(
            APPEND public_headers
#            util/threading-windows.h
#            util/windows/ComPtr.hpp
#            util/windows/CoTaskMemPtr.hpp
#            util/windows/device-enum.h
#            util/windows/HRError.hpp
            util/windows/win-registry.h
            util/windows/win-version.h
#            util/windows/window-helpers.h
#            util/windows/WinHandle.hpp
    )
#elseif(OS_MACOS)
#    list(APPEND public_headers util/apple/cfstring-utils.h)
endif()

#if(ENABLE_HEVC)
#    list(APPEND public_headers vura-hevc.h)
#endif()

set_property(TARGET libvura APPEND PROPERTY VURA_PUBLIC_HEADERS ${public_headers})

set_target_properties(
        libvura
        PROPERTIES FOLDER core
        VERSION 0
        SOVERSION "${VURA_VERSION_MAJOR}"
)

#target_export(libvura)
