# CMake Utitlity Functions

add_custom_command(
    TARGET vura POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/AUTHORS" "$<TARGET_FILE_DIR:vura>/assets/data/AUTHORS"
    COMMENT "Copying AUTHORS file to build directory..."
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
        ${CMAKE_SOURCE_DIR}/deps/ffmpeg/lib/avformat.lib
        ${CMAKE_SOURCE_DIR}/deps/ffmpeg/lib/avfilter.lib
        ${CMAKE_SOURCE_DIR}/deps/ffmpeg/lib/avcodec.lib
        ${CMAKE_SOURCE_DIR}/deps/ffmpeg/lib/avutil.lib
        ${CMAKE_SOURCE_DIR}/deps/ffmpeg/lib/swscale.lib
        ${CMAKE_SOURCE_DIR}/deps/ffmpeg/lib/swresample.lib
    )

    target_include_directories(vura PRIVATE ${CMAKE_SOURCE_DIR}/deps/ffmpeg/include)
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
