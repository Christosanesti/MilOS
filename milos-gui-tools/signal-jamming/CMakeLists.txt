cmake_minimum_required(VERSION 3.27)
project(MilosSignalJamming VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Quick QuickControls2 DBus Charts)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(APPLICATION_SOURCES
    src/main.cpp
    src/signal_jamming_system.cpp
    src/jamming_device.cpp
    src/jamming_manager.cpp
    src/spectrum_analyzer.cpp
    src/dbus_interface.cpp
    src/audit_logger.cpp
)

# Header files
set(APPLICATION_HEADERS
    include/signal_jamming_system.h
    include/jamming_device.h
    include/jamming_manager.h
    include/spectrum_analyzer.h
    include/dbus_interface.h
    include/audit_logger.h
)

# QML files
set(QML_FILES
    ui/main.qml
)

# Create executable
add_executable(milos-signal-jamming
    ${APPLICATION_SOURCES}
    ${APPLICATION_HEADERS}
)

# Enable MOC for Q_OBJECT classes
set_property(TARGET milos-signal-jamming PROPERTY AUTOMOC ON)

# Link libraries
target_link_libraries(milos-signal-jamming
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
    Qt6::Charts
)

# Add QML resources
qt_add_qml_module(milos-signal-jamming
    URI milos.signaljamming
    VERSION 1.0
    QML_FILES ${QML_FILES}
)

# Install targets
install(TARGETS milos-signal-jamming
    RUNTIME DESTINATION bin
)

