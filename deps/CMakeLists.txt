set(XXHASH_BUNDLED_MODE, OFF)
add_subdirectory(xxHash/build/cmake)

if (OS_WINDOWS)
    add_subdirectory(blake2)
    add_subdirectory(json11)
endif ()
