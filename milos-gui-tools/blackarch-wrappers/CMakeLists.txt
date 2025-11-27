cmake_minimum_required(VERSION 3.27)
project(blackarch-wrappers)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick QuickControls2)

# Find KDE Frameworks
find_package(KF6 REQUIRED COMPONENTS CoreAddons)

# Add logging library
add_subdirectory(../../base/libs/logging ${CMAKE_BINARY_DIR}/libs/logging)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files for wrapper framework
set(WRAPPER_FRAMEWORK_SOURCES
    src/wrapper_base.cpp
    src/tool_executor.cpp
)

set(WRAPPER_FRAMEWORK_HEADERS
    include/wrapper_base.h
    include/tool_executor.h
)

# Create wrapper framework library
add_library(blackarch-wrappers-framework STATIC
    ${WRAPPER_FRAMEWORK_SOURCES}
    ${WRAPPER_FRAMEWORK_HEADERS}
)

# Link libraries
target_link_libraries(blackarch-wrappers-framework
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    KF6::CoreAddons
    milos-logging
)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)
set_property(TARGET blackarch-wrappers-framework PROPERTY AUTOMOC ON)

# Install
install(TARGETS blackarch-wrappers-framework
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${WRAPPER_FRAMEWORK_HEADERS}
    DESTINATION include/blackarch-wrappers
)

