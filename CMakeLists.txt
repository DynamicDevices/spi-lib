cmake_minimum_required(VERSION 3.14)
project(spi-dev)

set(STV_3RD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rd_party")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake;${CMAKE_MODULE_PATH}")

## Add sdk_base_obj
set(RDK_HAS_DEVICE_CONTROL FALSE)
add_subdirectory("${STV_3RD_PARTY_DIR}/ersatzRDK" EXCLUDE_FROM_ALL)

## determine the target platform
include(TargetPlatform)
detect_target_platform(TARGET_PLATFORM)
message(STATUS "Target Platform: ${TARGET_PLATFORM}")

add_library(interface_app INTERFACE)
target_include_directories(interface_app INTERFACE public)

## define an app_stump library, that is almost the complete application except it is
## missing the acquisitions part which needs to be linked against it to create the full application
file(GLOB app_main_src
	modules/app/main/*.c modules/app/main/*.h
	)

add_library(app_main_lib STATIC ${app_main_src})
target_link_libraries(app_main_lib PUBLIC interface_app sdk_base_obj)
	
file(GLOB app_framework_src
		public/interface/*.h
		modules/app/framework/*.c modules/app/framework/*.h
	)

add_library(app_framework_lib STATIC ${app_framework_src})
target_link_libraries(app_framework_lib PUBLIC interface_app sdk_base_obj)
	
file(GLOB app_stump_src
		public/interface/*.h
		modules/record/plaintext/*.c modules/record/plaintext/*.h
		modules/report/console_json/*.c modules/report/console_json/*.h
	)


add_library(app_stump_lib STATIC ${app_stump_src})
#target_include_directories(app_stump_lib PUBLIC algo)
target_link_libraries(app_stump_lib PUBLIC app_framework_lib interface_app)

# library for direct interface data acquisition
add_library(interface_direct INTERFACE)
target_include_directories(interface_direct INTERFACE modules/lib/direct/public)

file(GLOB direct_src
	modules/lib/direct/*.c
	modules/lib/direct/*.cpp
	modules/lib/direct/*.hpp
	modules/lib/direct/driver/*.c
	modules/lib/direct/driver/*.h
)
add_library(lib_direct STATIC ${direct_src})
target_include_directories(lib_direct PRIVATE modules/lib/direct)
target_link_libraries(lib_direct PRIVATE interface_app)
target_link_libraries(lib_direct PUBLIC sdk_base_obj interface_direct)

file(GLOB acq_spi_src
	 modules/acquisition/spi/*.c
	)
add_library(lib_acq_spi STATIC ${acq_spi_src})
target_link_libraries(lib_acq_spi PUBLIC interface_app interface_direct sdk_base_obj)

# executable for data acquisition via /dev/spi on linux
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")

	file(GLOB lib_dev_spi_src
		modules/lib/dev_spi/*.c
		modules/lib/dev_spi/*.h
	)
	add_library(lib_dev_spi STATIC ${lib_dev_spi_src})
	target_link_libraries(lib_dev_spi PUBLIC interface_app interface_direct)

	
	add_executable(seamless_dev_spi ${app_main_src})
	target_link_libraries(seamless_dev_spi PUBLIC app_stump_lib)
	target_link_libraries(seamless_dev_spi PUBLIC lib_acq_spi lib_direct lib_dev_spi)
	target_link_libraries(seamless_dev_spi PUBLIC pthread)
endif ()
