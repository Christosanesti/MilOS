cmake_minimum_required(VERSION 3.27)
project(NetworkDashboardTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Test)
find_package(yaml-cpp REQUIRED)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Packet Capture Tests
add_executable(test_packet_capture test_packet_capture.cpp)
target_link_libraries(test_packet_capture
    Qt6::Core
    Qt6::Test
)
if(LIBPCAP_FOUND)
    target_link_libraries(test_packet_capture ${LIBPCAP_LIBRARIES})
    target_include_directories(test_packet_capture PRIVATE ${LIBPCAP_INCLUDE_DIRS})
    target_compile_definitions(test_packet_capture PRIVATE HAVE_LIBPCAP)
endif()
add_test(NAME PacketCaptureTest COMMAND test_packet_capture)

# Packet Statistics Tests
add_executable(test_packet_statistics test_packet_statistics.cpp)
target_link_libraries(test_packet_statistics
    Qt6::Core
    Qt6::Test
)
add_test(NAME PacketStatisticsTest COMMAND test_packet_statistics)

# Config Parser Tests
add_executable(test_config_parser test_config_parser.cpp)
target_link_libraries(test_config_parser
    Qt6::Core
    Qt6::Test
    yaml-cpp::yaml-cpp
)
add_test(NAME ConfigParserTest COMMAND test_config_parser)

