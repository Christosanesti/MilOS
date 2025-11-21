cmake_minimum_required(VERSION 3.27)
project(MilosAdvancedNetworkDefense VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Quick QuickControls2 DBus Sql)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(APPLICATION_SOURCES
    src/main.cpp
    src/advanced_network_defense_system.cpp
    src/ml_threat_detector.cpp
    src/network_forensics.cpp
    src/packet_analyzer.cpp
    src/ids_rules_engine.cpp
    src/incident_response_workflow.cpp
    src/dbus_interface.cpp
    src/audit_logger.cpp
)

# Header files
set(APPLICATION_HEADERS
    include/advanced_network_defense_system.h
    include/ml_threat_detector.h
    include/network_forensics.h
    include/packet_analyzer.h
    include/ids_rules_engine.h
    include/incident_response_workflow.h
    include/dbus_interface.h
    include/audit_logger.h
)

# QML files
set(QML_FILES
    ui/main.qml
)

# Create executable
add_executable(milos-advanced-network-defense
    ${APPLICATION_SOURCES}
    ${APPLICATION_HEADERS}
)

# Enable MOC for Q_OBJECT classes
set_property(TARGET milos-advanced-network-defense PROPERTY AUTOMOC ON)

# Link libraries
target_link_libraries(milos-advanced-network-defense
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
    Qt6::Sql
)

# Add QML resources
qt_add_qml_module(milos-advanced-network-defense
    URI milos.advancednetworkdefense
    VERSION 1.0
    QML_FILES ${QML_FILES}
)

# Install targets
install(TARGETS milos-advanced-network-defense
    RUNTIME DESTINATION bin
)

