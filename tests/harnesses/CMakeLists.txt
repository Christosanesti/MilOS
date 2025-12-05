cmake_minimum_required(VERSION 3.16)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core DBus Test)

# Logging library is added at the top level

# D-Bus Test Harness Library
add_library(dbus-test-harness STATIC
    dbus_test_harness.cpp
    dbus_test_harness.h
)

target_link_libraries(dbus-test-harness
    Qt6::Core
    Qt6::DBus
    Qt6::Test
    milos-logging
)

set_property(TARGET dbus-test-harness PROPERTY AUTOMOC ON)

# Install harness for use in other tests
install(TARGETS dbus-test-harness
    ARCHIVE DESTINATION lib
)

install(FILES dbus_test_harness.h
    DESTINATION include/milos/testing
)

