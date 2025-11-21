cmake_minimum_required(VERSION 3.27)
project(MilosMilitaryRadio VERSION 1.0.0 LANGUAGES CXX)

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
    src/military_radio_system.cpp
    src/radio_protocol.cpp
    src/radio_encryption.cpp
    src/radio_manager.cpp
    src/dbus_interface.cpp
    src/audit_logger.cpp
)

# Header files
set(APPLICATION_HEADERS
    include/military_radio_system.h
    include/radio_protocol.h
    include/radio_encryption.h
    include/radio_manager.h
    include/dbus_interface.h
    include/audit_logger.h
)

# QML files
set(QML_FILES
    ui/main.qml
)

# Create executable
add_executable(milos-military-radio
    ${APPLICATION_SOURCES}
    ${APPLICATION_HEADERS}
)

# Enable MOC for Q_OBJECT classes
set_property(TARGET milos-military-radio PROPERTY AUTOMOC ON)

# Link libraries
target_link_libraries(milos-military-radio
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
)

# Add QML resources
qt_add_qml_module(milos-military-radio
    URI milos.militaryradio
    VERSION 1.0
    QML_FILES ${QML_FILES}
)

# Install targets
install(TARGETS milos-military-radio
    RUNTIME DESTINATION bin
)

