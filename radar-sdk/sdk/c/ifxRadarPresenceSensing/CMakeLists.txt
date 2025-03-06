set(presence_lib "internal/xensiv-radar-presence")
set(sensor_dsp_lib "internal/sensor-dsp")

file(GLOB sdk_radar_presence_sensing_files *.c *.h
    ${presence_lib}/*.h    
    ${presence_lib}/*.c
    ${sensor_dsp_lib}/include/*.h
    ${sensor_dsp_lib}/source/*.c
    )

add_library(sdk_presence_sensing SHARED ${sdk_radar_presence_sensing_files})
set_property(TARGET sdk_presence_sensing PROPERTY C_STANDARD 11)
target_link_libraries(sdk_presence_sensing PUBLIC sdk_base)
target_link_libraries(sdk_presence_sensing PRIVATE CMSISDSP)
target_include_directories(sdk_presence_sensing PRIVATE . "${sensor_dsp_lib}/include" "${presence_lib}")
