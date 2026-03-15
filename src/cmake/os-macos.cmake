include(cmake/feature-sparkle.cmake)

target_compile_options(vura PRIVATE -Wno-quoted-include-in-framework-header -Wno-comma)

list(APPEND _frontend_objcxx_compile_options -fobjc-arc -fmodules -fcxx-modules)

if(CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 14.0.3)
    target_compile_options(vura PRIVATE -Wno-error=unqualified-std-cast-call)
endif()

target_link_libraries(
        vura
        PRIVATE
        "$<LINK_LIBRARY:FRAMEWORK,AppKit.framework>"
        "$<LINK_LIBRARY:FRAMEWORK,ApplicationServices.framework>"
        "$<LINK_LIBRARY:FRAMEWORK,AVFoundation.framework>"
)

qt_add_ios_ffmpeg_libraries(vura)

set_target_properties(vura PROPERTIES MACOSX_BUNDLE TRUE)
