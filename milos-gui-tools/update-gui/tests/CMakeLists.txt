cmake_minimum_required(VERSION 3.27)

# Find Qt6 Test and DBus
find_package(Qt6 REQUIRED COMPONENTS Test DBus)

# Create test executable
add_executable(milos-update-gui-tests
    test_main.cpp
    ../src/dbus_interface_wrapper.cpp
    ../src/update_manager.cpp
)

# Include directories
target_include_directories(milos-update-gui-tests PRIVATE
    ../include
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Link libraries
target_link_libraries(milos-update-gui-tests
    Qt6::Core
    Qt6::Test
    Qt6::DBus
)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)
set_property(TARGET milos-update-gui-tests PROPERTY AUTOMOC ON)

# Add test
add_test(NAME UpdateGuiTests COMMAND milos-update-gui-tests)


