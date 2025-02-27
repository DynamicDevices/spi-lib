
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(sensor_sdk SHARED ../dummy.c)
target_link_libraries(sensor_sdk PUBLIC radar_sdk)

add_library(sensor_sdk_static STATIC ../dummy.c)
target_compile_definitions(sensor_sdk_static PUBLIC RADAR_SDK_BUILD_STATIC=1)
target_link_libraries(sensor_sdk_static PUBLIC radar_sdk)
