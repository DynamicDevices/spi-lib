
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(radar_sdk SHARED ../dummy.c)
target_link_libraries(radar_sdk PUBLIC
    sdk_mimose
    sdk_ltr11
    sdk_radar
    sdk_avian
    sdk_fmcw
    sdk_cw
    sdk_presence_sensing
)
if(WIN32 OR MINGW OR MSYS)
    # on Windows the library should not have the prefix lib
    set_target_properties(radar_sdk PROPERTIES PREFIX "")
endif()
