cmake_minimum_required(VERSION 3.27)

# Find Qt6 Test
find_package(Qt6 REQUIRED COMPONENTS Test)

# Test executable
add_executable(milos-audit-gui-tests
    test_main.cpp
)

# Link libraries
target_link_libraries(milos-audit-gui-tests
    Qt6::Core
    Qt6::Test
    Qt6::DBus
)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)
set_property(TARGET milos-audit-gui-tests PROPERTY AUTOMOC ON)

# Add test to CTest
add_test(NAME milos-audit-gui-tests COMMAND milos-audit-gui-tests)

