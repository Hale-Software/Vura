if (ENABLE_TESTS)

    if (OS_WINDOWS)
        add_subdirectory(windows)
    endif ()

    if (OS_MACOS)
        add_subdirectory(osx)
    endif ()

endif ()