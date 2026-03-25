add_library(libvura-version OBJECT)
add_library(VURA::libvura-version ALIAS libvura-version)

configure_file(vuraversion.c.in vuraversion.c @ONLY)

target_sources(libvura-version PRIVATE vuraversion.c PUBLIC vuraversion.h)

target_include_directories(libvura-version PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")

set_property(TARGET libvura-version PROPERTY FOLDER core)
