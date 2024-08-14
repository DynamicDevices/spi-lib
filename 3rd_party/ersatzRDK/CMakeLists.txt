#cmake_minimum_required(VERSION 3.12)
include(CheckLibraryExists)

option(SDK_ENABLE_LOGS     "enable logging with debug log level " OFF)

if(SDK_ENABLE_LOGS)
    add_definitions(-DIFX_LOG_SEVERITY_DEBUG=1)
endif()

# Check if it is necessary to link against libm
check_library_exists(m sqrt "" HAS_LIBM)

add_subdirectory(ifxBase)

