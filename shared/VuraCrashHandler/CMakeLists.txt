cmake_minimum_required(VERSION 3.16)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

add_library(VuruCrashHandler STATIC)
add_library(VURA::VuruCrashHandler ALIAS VuruCrashHandler)

set(BREAKPAD_PATH "third_party/breakpad/src")

set_target_properties(VuruCrashHandler PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

target_sources(
        VuruCrashHandler
        PUBLIC
        QBreakpadHandler.h
        QBreakpadHttpUploader.h
        singletone/call_once.h
        singletone/singleton.h

        PRIVATE
        QBreakpadHandler.cpp
        QBreakpadHttpUploader.cpp
)

if(UNIX)
    target_sources(VuruCrashHandler PRIVATE
            ${BREAKPAD_PATH}/client/minidump_file_writer.cc
            ${BREAKPAD_PATH}/common/convert_UTF.cc
            ${BREAKPAD_PATH}/common/md5.cc
            ${BREAKPAD_PATH}/common/string_conversion.cc
    )
endif()

if(APPLE)
    enable_language(OBJCXX)

    target_sources(VuruCrashHandler PRIVATE
            ${BREAKPAD_PATH}/client/mac/crash_generation/crash_generation_client.cc
            ${BREAKPAD_PATH}/client/mac/handler/breakpad_nlist_64.cc
            ${BREAKPAD_PATH}/client/mac/handler/dynamic_images.cc
            ${BREAKPAD_PATH}/client/mac/handler/exception_handler.cc
            ${BREAKPAD_PATH}/client/mac/handler/minidump_generator.cc
            ${BREAKPAD_PATH}/common/mac/MachIPC.mm
            ${BREAKPAD_PATH}/common/mac/bootstrap_compat.cc
            ${BREAKPAD_PATH}/common/mac/file_id.cc
            ${BREAKPAD_PATH}/common/mac/macho_id.cc
            ${BREAKPAD_PATH}/common/mac/macho_utilities.cc
            ${BREAKPAD_PATH}/common/mac/macho_walker.cc
            ${BREAKPAD_PATH}/common/mac/string_utilities.cc
    )
endif()

if(UNIX AND NOT APPLE)
    target_sources(VuruCrashHandler PRIVATE
            ${BREAKPAD_PATH}/client/linux/crash_generation/crash_generation_client.cc
            ${BREAKPAD_PATH}/client/linux/dump_writer_common/thread_info.cc
            ${BREAKPAD_PATH}/client/linux/dump_writer_common/ucontext_reader.cc
            ${BREAKPAD_PATH}/client/linux/handler/exception_handler.cc
            ${BREAKPAD_PATH}/client/linux/handler/minidump_descriptor.cc
            ${BREAKPAD_PATH}/client/linux/log/log.cc
            ${BREAKPAD_PATH}/client/linux/microdump_writer/microdump_writer.cc
            ${BREAKPAD_PATH}/client/linux/minidump_writer/linux_core_dumper.cc
            ${BREAKPAD_PATH}/client/linux/minidump_writer/linux_dumper.cc
            ${BREAKPAD_PATH}/client/linux/minidump_writer/linux_ptrace_dumper.cc
            ${BREAKPAD_PATH}/client/linux/minidump_writer/minidump_writer.cc
            ${BREAKPAD_PATH}/common/linux/breakpad_getcontext.S
            ${BREAKPAD_PATH}/common/linux/elfutils.cc
            ${BREAKPAD_PATH}/common/linux/file_id.cc
            ${BREAKPAD_PATH}/common/linux/guid_creator.cc
            ${BREAKPAD_PATH}/common/linux/linux_libc_support.cc
            ${BREAKPAD_PATH}/common/linux/memory_mapped_file.cc
            ${BREAKPAD_PATH}/common/linux/safe_readlink.cc
    )
endif()

if(WIN32)
    target_sources(VuruCrashHandler PRIVATE
            ${BREAKPAD_PATH}/client/windows/crash_generation/crash_generation_client.cc
            ${BREAKPAD_PATH}/client/windows/handler/exception_handler.cc
            ${BREAKPAD_PATH}/common/windows/guid_string.cc
    )
endif()

target_link_libraries(VuruCrashHandler PRIVATE Qt6::Core Qt6::Network)
target_include_directories(VuruCrashHandler INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} PUBLIC ${BREAKPAD_PATH})
