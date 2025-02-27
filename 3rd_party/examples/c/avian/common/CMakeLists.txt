set(EXAMPLE_COMMON_SOURCES
    common.c
    json.cpp
    time_formatter.c
    util.c
)

set(EXAMPLE_COMMON_HEADERS
    common.h
    json.h
    json.hpp
    time_formatter.h
    util.h
)

add_library(example_common STATIC ${EXAMPLE_COMMON_SOURCES} ${EXAMPLE_COMMON_HEADERS})
target_include_directories(example_common PUBLIC .)
target_link_libraries(example_common PUBLIC sdk_avian sdk_radar)
target_link_libraries(example_common PRIVATE argparse nlohmann_json)
