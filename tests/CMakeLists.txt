cmake_minimum_required(VERSION 3.27)

# Find Qt6 Test
find_package(Qt6 REQUIRED COMPONENTS Core Test DBus)

# Integration Tests
add_executable(test_system_integration
    integration/test_system_integration.cpp
)

target_link_libraries(test_system_integration
    Qt6::Core
    Qt6::Test
    Qt6::DBus
)

# Performance Tests
add_executable(test_performance
    performance/test_performance.cpp
)

target_link_libraries(test_performance
    Qt6::Core
    Qt6::Test
    Qt6::DBus
)

# Accessibility Tests
add_executable(test_accessibility
    accessibility/test_accessibility.cpp
)

target_link_libraries(test_accessibility
    Qt6::Core
    Qt6::Test
    Qt6::Widgets
)

# Security Tests
add_executable(test_security
    security/test_security.cpp
)

target_link_libraries(test_security
    Qt6::Core
    Qt6::Test
    Qt6::DBus
)

# Add tests to CTest
add_test(NAME SystemIntegrationTest COMMAND test_system_integration)
add_test(NAME PerformanceTest COMMAND test_performance)
add_test(NAME AccessibilityTest COMMAND test_accessibility)
add_test(NAME SecurityTest COMMAND test_security)

