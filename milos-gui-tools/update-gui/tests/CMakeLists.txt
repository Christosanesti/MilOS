cmake_minimum_required(VERSION 3.27)

# Find Qt6 Test
find_package(Qt6 REQUIRED COMPONENTS Test)

# Create test executable
add_executable(milos-update-gui-tests
    test_main.cpp
)

# Link libraries
target_link_libraries(milos-update-gui-tests
    Qt6::Core
    Qt6::Test
)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)
set_property(TARGET milos-update-gui-tests PROPERTY AUTOMOC ON)

# Add test
add_test(NAME UpdateGuiTests COMMAND milos-update-gui-tests)

