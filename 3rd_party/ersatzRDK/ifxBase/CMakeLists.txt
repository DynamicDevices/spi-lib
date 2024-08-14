file(GLOB sdk_base_src *.c *.h *.cpp *.hpp internal/*.c internal/*.h)
add_library(sdk_base_obj OBJECT ${sdk_base_src})
target_include_directories(sdk_base_obj PUBLIC ..)
#target_compile_definitions(sdk_base_obj PRIVATE radar_sdk_EXPORTS=1)
target_compile_definitions(sdk_base_obj PUBLIC RADAR_SDK_BUILD_STATIC)

if(HAS_LIBM)
    target_link_libraries(sdk_base_obj PUBLIC m)
endif()
