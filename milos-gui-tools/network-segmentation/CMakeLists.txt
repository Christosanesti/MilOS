cmake_minimum_required(VERSION 3.27)
project(network-segmentation)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick QuickControls2 DBus)

# Find KDE Frameworks
find_package(KF6 REQUIRED COMPONENTS CoreAddons)

# Find CLI11 for CLI tool
find_package(CLI11 REQUIRED)

# Find SQLite3 for segment storage
find_package(SQLite3 REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(APPLICATION_SOURCES
    src/main.cpp
    src/network_segmentation.cpp
    src/segment_manager.cpp
    src/segment_config.cpp
    src/topology_display.cpp
    src/firewall_rule_generator.cpp
    src/firewall_rule_validator.cpp
    src/firewall_manager.cpp
    src/dbus_interface.cpp
    src/cli_tool.cpp
    src/audit_logger.cpp
)

set(APPLICATION_HEADERS
    include/network_segmentation.h
    include/segment_manager.h
    include/segment_config.h
    include/topology_display.h
    include/firewall_rule_generator.h
    include/firewall_rule_validator.h
    include/firewall_manager.h
    include/dbus_interface.h
    include/audit_logger.h
)

# QML files
set(QML_FILES
    ui/main.qml
    ui/SegmentList.qml
    ui/SegmentEditor.qml
    ui/TopologyView.qml
    ui/SegmentCard.qml
)

# Create executable
add_executable(milos-network-segmentation
    ${APPLICATION_SOURCES}
    ${APPLICATION_HEADERS}
    ${QML_FILES}
)

# Link libraries
target_link_libraries(milos-network-segmentation
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
    KF6::CoreAddons
    CLI11::CLI11
    SQLite3::SQLite3
)

# Enable Qt MOC and QRC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set_property(TARGET milos-network-segmentation PROPERTY AUTOMOC ON)

# Install
install(TARGETS milos-network-segmentation
    RUNTIME DESTINATION bin
)

