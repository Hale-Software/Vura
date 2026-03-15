# 1. CONFIG += static
# This ensures the target is built as a static library (.lib or .a)
set_target_properties(qBreakpad PROPERTIES
        LINK_SEARCH_START_STATIC ON
        LINK_SEARCH_END_STATIC ON
)

# 2. macx { CONFIG += c++11; LIBS += -lcrypto }
if(APPLE)
    set_target_properties(qBreakpad PROPERTIES
            CXX_STANDARD 11
            CXX_STANDARD_REQUIRED ON
    )
    # Find and link the crypto library
    find_package(OpenSSL REQUIRED)
    target_link_libraries(qBreakpad PRIVATE OpenSSL::Crypto)
endif()

# 3. Translation of the CONFIG loop/warnings
# In CMake, we check for these features using target properties or compile options.
# Since these are typically standard now, we enforce them directly:
target_compile_options(qBreakpad PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/EHsc> # Enable Exceptions for MSVC
        $<$<CXX_COMPILER_ID:GNU,Clang>:-fexceptions -frtti> # Enable Exceptions/RTTI for GCC/Clang
)

# 4. DEFINES += CALIPER
target_compile_definitions(qBreakpad PUBLIC CALIPER)