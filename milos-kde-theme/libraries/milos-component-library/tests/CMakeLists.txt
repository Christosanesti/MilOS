cmake_minimum_required(VERSION 3.27)

find_package(Qt6 REQUIRED COMPONENTS Core Test Quick)

# Button component tests
add_executable(test_button
    test_button.cpp
    ../src/components/Button.cpp
)

target_link_libraries(test_button
    Qt6::Core
    Qt6::Test
    Qt6::Quick
)

# Integration tests
add_executable(test_integration
    test_integration.cpp
)

target_link_libraries(test_integration
    Qt6::Core
    Qt6::Test
    Qt6::Quick
)

# Component specification verification tests
add_executable(test_component_specification_verification
    test_component_specification_verification.cpp
)

target_link_libraries(test_component_specification_verification
    Qt6::Core
    Qt6::Test
    Qt6::Quick
)

# Accessibility compliance tests
add_executable(test_accessibility_compliance
    test_accessibility_compliance.cpp
)

target_link_libraries(test_accessibility_compliance
    Qt6::Core
    Qt6::Test
    Qt6::Quick
)

# Add tests
add_test(NAME ButtonComponentTest COMMAND test_button)
add_test(NAME ComponentIntegrationTest COMMAND test_integration)
add_test(NAME ComponentSpecificationVerificationTest COMMAND test_component_specification_verification)
add_test(NAME AccessibilityComplianceTest COMMAND test_accessibility_compliance)

