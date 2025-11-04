cmake_minimum_required(VERSION 3.27)
project(EmergencyWidgetTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Test configuration
find_package(Qt6 REQUIRED COMPONENTS Core Test Quick)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

# Emergency Widget test executable
add_executable(emergencywidget_test
    emergencywidget_test.cpp
)

# Link Qt6 libraries for emergencywidget_test
target_link_libraries(emergencywidget_test
    Qt6::Core
    Qt6::Test
    Qt6::Quick
)

# Add emergency widget test
add_test(NAME EmergencyWidgetTest COMMAND emergencywidget_test)

