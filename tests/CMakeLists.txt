cmake_minimum_required(VERSION 3.27)
project(MilOSTests)

# Find Qt6 Test
find_package(Qt6 REQUIRED COMPONENTS Core Test DBus Widgets)

# Add logging library once for all tests
add_subdirectory(../milos-base/libs/logging ${CMAKE_BINARY_DIR}/libs/logging)

# Unit Tests
add_subdirectory(unit)

# Test Harnesses
add_subdirectory(harnesses)

# Integration Tests
add_executable(test_system_integration
    integration/test_system_integration.cpp
)

target_link_libraries(test_system_integration
    Qt6::Core
    Qt6::Test
    Qt6::DBus
)
set_property(TARGET test_system_integration PROPERTY AUTOMOC ON)

# Performance Tests
add_executable(test_performance
    performance/test_performance.cpp
)

target_link_libraries(test_performance
    Qt6::Core
    Qt6::Test
    Qt6::DBus
)
set_property(TARGET test_performance PROPERTY AUTOMOC ON)

# Accessibility Tests
add_executable(test_accessibility
    accessibility/test_accessibility.cpp
)

target_link_libraries(test_accessibility
    Qt6::Core
    Qt6::Test
    Qt6::Widgets
)
set_property(TARGET test_accessibility PROPERTY AUTOMOC ON)

# Security Tests
add_executable(test_security
    security/test_security.cpp
)

target_link_libraries(test_security
    Qt6::Core
    Qt6::Test
    Qt6::DBus
)
set_property(TARGET test_security PROPERTY AUTOMOC ON)

# Add tests to CTest
add_test(NAME SystemIntegrationTest COMMAND test_system_integration)
add_test(NAME PerformanceTest COMMAND test_performance)
add_test(NAME AccessibilityTest COMMAND test_accessibility)
add_test(NAME SecurityTest COMMAND test_security)

