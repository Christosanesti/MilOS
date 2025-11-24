cmake_minimum_required(VERSION 3.27)

# Find Qt6 Test
find_package(Qt6 REQUIRED COMPONENTS Test Quick Qml)

# Test executable
add_executable(alert-zone-tests
    alertzonewidget_test.cpp
)

# Link libraries
target_link_libraries(alert-zone-tests
    Qt6::Core
    Qt6::Test
    Qt6::Quick
    Qt6::Qml
    alert-zone-services
)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)
set_property(TARGET alert-zone-tests PROPERTY AUTOMOC ON)

# Add test to CTest
add_test(NAME alert-zone-tests COMMAND alert-zone-tests)

