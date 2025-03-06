add_executable(example_presence_detection presence_detection.c)
target_link_libraries(example_presence_detection example_common sdk_presence_sensing)
target_include_directories(example_presence_detection PRIVATE . .. "../common" "../../../../ersatzRDK" "../../../../3rd_party/libs/nlohmann" )
