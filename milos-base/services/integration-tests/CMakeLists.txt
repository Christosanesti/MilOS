cmake_minimum_required(VERSION 3.27)
project(ServiceIntegrationTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Test DBus)
find_package(SQLite3 REQUIRED)
find_package(yaml-cpp REQUIRED)

# Include directories for service headers
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../data-guard/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../audit-service/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../update-service/src
)

# Service Integration Tests
add_executable(test_service_integration test_service_integration.cpp)
target_link_libraries(test_service_integration
    Qt6::Core
    Qt6::Test
    Qt6::DBus
    SQLite::SQLite3
    yaml-cpp::yaml-cpp
)
add_test(NAME ServiceIntegrationTest COMMAND test_service_integration)

# D-Bus Interface Tests
add_executable(test_dbus_interfaces test_dbus_interfaces.cpp)
target_link_libraries(test_dbus_interfaces
    Qt6::Core
    Qt6::Test
    Qt6::DBus
)
add_test(NAME DBusInterfaceTest COMMAND test_dbus_interfaces)

# CLI Tool Tests
add_executable(test_cli_tools test_cli_tools.cpp)
target_link_libraries(test_cli_tools
    Qt6::Core
    Qt6::Test
)
add_test(NAME CLIToolTest COMMAND test_cli_tools)

# Service Isolation Tests (documentation and structure)
# Note: Full isolation tests require systemd environment with MAC policies

# Graceful Degradation Tests
add_executable(test_graceful_degradation test_graceful_degradation.cpp)
target_link_libraries(test_graceful_degradation
    Qt6::Core
    Qt6::Test
    Qt6::DBus
    SQLite::SQLite3
    yaml-cpp::yaml-cpp
)
add_test(NAME GracefulDegradationTest COMMAND test_graceful_degradation)

