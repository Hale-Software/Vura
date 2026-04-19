cmake_minimum_required(VERSION 3.21)

project(tests VERSION 0.1 LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Test)

qt_standard_project_setup(REQUIRES 6.5)


qt_add_executable(CoreTest
    tst_coretest.cpp
)
add_test(NAME CoreTest COMMAND CoreTest)


set_target_properties(CoreTest PROPERTIES
    MACOSX_BUNDLE FALSE
    WIN32_EXECUTABLE FALSE
)

target_link_libraries(CoreTest
    PRIVATE Qt6::Test
)
