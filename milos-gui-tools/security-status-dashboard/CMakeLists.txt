cmake_minimum_required(VERSION 3.27)
project(security-status-dashboard)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick QuickControls2 DBus)

# Find KDE Frameworks
find_package(KF6 REQUIRED COMPONENTS CoreAddons)

# Find CLI11 for CLI tool
find_package(CLI11 REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(APPLICATION_SOURCES
    src/main.cpp
    src/security_status_dashboard.cpp
    src/status_aggregator.cpp
    src/dbus_interface.cpp
    src/cli_tool.cpp
    src/audit_logger.cpp
)

set(APPLICATION_HEADERS
    include/security_status_dashboard.h
    include/status_aggregator.h
    include/dbus_interface.h
    include/audit_logger.h
)

# QML files
set(QML_FILES
    ui/main.qml
    ui/SecurityPostureCard.qml
    ui/HardeningStatusCard.qml
    ui/EncryptionStatusCard.qml
    ui/NetworkSecurityStatusCard.qml
    ui/UpdateStatusCard.qml
    ui/StatusIndicator.qml
)

# Create executable
add_executable(milos-security-status-dashboard
    ${APPLICATION_SOURCES}
    ${APPLICATION_HEADERS}
    ${QML_FILES}
)

# Link libraries
target_link_libraries(milos-security-status-dashboard
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
    KF6::CoreAddons
    CLI11::CLI11
)

# Enable Qt MOC and QRC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set_property(TARGET milos-security-status-dashboard PROPERTY AUTOMOC ON)

# Install
install(TARGETS milos-security-status-dashboard
    RUNTIME DESTINATION bin
)

