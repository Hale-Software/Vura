include_guard(GLOBAL)

# Define project version (required by CPack)
set(CPACK_PACKAGE_VERSION_MAJOR VURA_VERSION_MAJOR)
set(CPACK_PACKAGE_VERSION_MINOR VURA_VERSION_MINOR)
set(CPACK_PACKAGE_VERSION_PATCH VURA_VERSION_PATCH)

# Include the CPack module to generate config files
include(CPack)
