
# TODO: We are currently linking all needed stratula libraries to the protocol library. This is not ideal and should be fixed.

file(GLOB protocol_src  *.c *.h 
                        serial/*.c serial/*.h requests/*.c requests/*.h 
                        ../*.h ../*.c
                        ../platform/*.h ../platform/*.c
                        ../components/*.h ../components/*.c
                        ../components/radar/Avian*.h ../components/radar/Avian*.c
                        ../components/radar/avian/*.h ../components/radar/avian/*.c
                        ../board/*.h ../board/*.c
                        ../common/crc/*.c ../common/crc/*.h 
                        ../impl/peripherals/*.h ../impl/peripherals/*.c ../impl/serial/*.h ../impl/serial/*.c 
)

add_library(protocol_obj OBJECT ${protocol_src})
target_include_directories(protocol_obj PUBLIC ..)
#target_compile_definitions(protocol_obj PUBLIC RADAR_SDK_BUILD_STATIC)

#if(HAS_LIBM)
#    target_link_libraries(sdk_base_obj PUBLIC m)
#endif()
