cmake_minimum_required(VERSION 3.27)

find_package(Qt6 REQUIRED COMPONENTS Core Test)
find_package(yaml-cpp REQUIRED)

# Policy Manager tests
add_executable(test_policy_manager
    test_policy_manager.cpp
    ../src/policy_manager.cpp
    ../src/config_parser.cpp
)

target_link_libraries(test_policy_manager
    Qt6::Core
    Qt6::Test
    yaml-cpp::yaml-cpp
)

set_property(TARGET test_policy_manager PROPERTY AUTOMOC ON)

# Config Parser tests
add_executable(test_config_parser
    test_config_parser.cpp
    ../src/config_parser.cpp
)

target_link_libraries(test_config_parser
    Qt6::Core
    Qt6::Test
    yaml-cpp::yaml-cpp
)

set_property(TARGET test_config_parser PROPERTY AUTOMOC ON)

# Network Enforcement tests
add_executable(test_network_enforcement
    test_network_enforcement.cpp
    ../src/network_enforcement.cpp
    ../src/policy_manager.cpp
    ../src/config_parser.cpp
)

target_link_libraries(test_network_enforcement
    Qt6::Core
    Qt6::Test
    yaml-cpp::yaml-cpp
    pthread
)

# Find libpcap
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBPCAP libpcap)
if(LIBPCAP_FOUND)
    target_link_libraries(test_network_enforcement ${LIBPCAP_LIBRARIES})
    target_include_directories(test_network_enforcement PRIVATE ${LIBPCAP_INCLUDE_DIRS})
endif()

set_property(TARGET test_network_enforcement PROPERTY AUTOMOC ON)

# Add tests to CTest
enable_testing()

add_test(NAME PolicyManagerTest COMMAND test_policy_manager)
add_test(NAME ConfigParserTest COMMAND test_config_parser)
add_test(NAME NetworkEnforcementTest COMMAND test_network_enforcement)

# Set test properties
set_tests_properties(PolicyManagerTest PROPERTIES
    TIMEOUT 30
    LABELS "unit;policy"
)

set_tests_properties(ConfigParserTest PROPERTIES
    TIMEOUT 30
    LABELS "unit;config"
)

set_tests_properties(NetworkEnforcementTest PROPERTIES
    TIMEOUT 60
    LABELS "unit;network"
)

