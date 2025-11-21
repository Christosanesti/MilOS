cmake_minimum_required(VERSION 3.27)
project(MilosThreatIntelligence VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Quick QuickControls2 DBus Network)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(APPLICATION_SOURCES
    src/main.cpp
    src/threat_intelligence_system.cpp
    src/threat_feed.cpp
    src/threat_correlator.cpp
    src/threat_blocker.cpp
    src/dbus_interface.cpp
    src/audit_logger.cpp
)

# Header files
set(APPLICATION_HEADERS
    include/threat_intelligence_system.h
    include/threat_feed.h
    include/threat_correlator.h
    include/threat_blocker.h
    include/dbus_interface.h
    include/audit_logger.h
)

# QML files
set(QML_FILES
    ui/main.qml
)

# Create executable
add_executable(milos-threat-intelligence
    ${APPLICATION_SOURCES}
    ${APPLICATION_HEADERS}
)

# Enable MOC for Q_OBJECT classes
set_property(TARGET milos-threat-intelligence PROPERTY AUTOMOC ON)

# Link libraries
target_link_libraries(milos-threat-intelligence
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
    Qt6::Network
)

# Add QML resources
qt_add_qml_module(milos-threat-intelligence
    URI milos.threatintelligence
    VERSION 1.0
    QML_FILES ${QML_FILES}
)

# Install targets
install(TARGETS milos-threat-intelligence
    RUNTIME DESTINATION bin
)

