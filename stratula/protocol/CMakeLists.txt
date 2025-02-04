file(GLOB protocol_src *.c *.h)
add_library(protocol_obj OBJECT ${protocol_src})
target_include_directories(protocol_obj PUBLIC ..)
#target_compile_definitions(protocol_obj PUBLIC RADAR_SDK_BUILD_STATIC)

#if(HAS_LIBM)
#    target_link_libraries(sdk_base_obj PUBLIC m)
#endif()
