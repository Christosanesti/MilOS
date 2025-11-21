cmake_minimum_required(VERSION 3.27)
project(MilosAISecurityMonitoring VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Quick QuickControls2 DBus)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(APPLICATION_SOURCES
    src/main.cpp
    src/ai_security_monitoring_system.cpp
    src/threat_detector.cpp
    src/incident_response.cpp
    src/system_monitor.cpp
    src/dbus_interface.cpp
    src/audit_logger.cpp
)

# Header files
set(APPLICATION_HEADERS
    include/ai_security_monitoring_system.h
    include/threat_detector.h
    include/incident_response.h
    include/system_monitor.h
    include/dbus_interface.h
    include/audit_logger.h
)

# QML files
set(QML_FILES
    ui/main.qml
)

# Create executable
add_executable(milos-ai-security-monitoring
    ${APPLICATION_SOURCES}
    ${APPLICATION_HEADERS}
)

# Enable MOC for Q_OBJECT classes
set_property(TARGET milos-ai-security-monitoring PROPERTY AUTOMOC ON)

# Link libraries
target_link_libraries(milos-ai-security-monitoring
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
)

# Add QML resources
qt_add_qml_module(milos-ai-security-monitoring
    URI milos.aisecuritymonitoring
    VERSION 1.0
    QML_FILES ${QML_FILES}
)

# Install targets
install(TARGETS milos-ai-security-monitoring
    RUNTIME DESTINATION bin
)

