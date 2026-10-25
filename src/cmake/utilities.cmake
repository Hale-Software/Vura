# CMake Utitlity Functions

add_custom_command(
    TARGET vura POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/AUTHORS" "$<TARGET_FILE_DIR:vura>/assets/data/AUTHORS"
    COMMENT "Copying AUTHORS file to build directory..."
)

find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
    libavcodec
    libavformat
    libavutil
    libswscale
    libswresample
)

target_link_libraries(vura PRIVATE VURA::libvura)

target_include_directories(
    vura PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/components
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/forms
    ${CMAKE_CURRENT_SOURCE_DIR}/settings
    ${CMAKE_CURRENT_SOURCE_DIR}/utility
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets
)

if (VURA_ENABLE_FFMPEG)
    target_link_libraries(
        vura PRIVATE
        PkgConfig::LIBAV
        #/opt/homebrew/opt/ffmpeg/lib/avformat.lib
        #/opt/homebrew/opt/ffmpeg/lib/avfilter.lib
        #/opt/homebrew/opt/ffmpeg/lib/avcodec.lib
        #/opt/homebrew/opt/ffmpeg/lib/avutil.lib
        #/opt/homebrew/opt/ffmpeg/lib/swscale.lib
        #/opt/homebrew/opt/ffmpeg/lib/swresample.lib
    )

    target_include_directories(vura PRIVATE ${LIBAV_INCLUDE_DIRS})
endif ()

file(GLOB_RECURSE _libvura_headers CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/libvura/libvura/*.h")

set_source_files_properties(${_libvura_headers} PROPERTIES SKIP_AUTOMOC ON)

set_target_properties(
    vura PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
    AUTOGEN_PARALLEL AUTO
    AUTOUIC_SEARCH_PATHS forms
)

set_property(TARGET vura APPEND PROPERTY AUTOUIC_SEARCH_PATHS forms)
