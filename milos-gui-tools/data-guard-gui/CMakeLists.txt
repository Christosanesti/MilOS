cmake_minimum_required(VERSION 3.27)
project(MilosDataGuardGui VERSION 1.0.0 LANGUAGES CXX)

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
    src/data_guard_manager.cpp
    src/dbus_interface_wrapper.cpp
)

set(APPLICATION_HEADERS
    include/data_guard_manager.h
    include/dbus_interface_wrapper.h
)

# QML files
set(QML_FILES
    ui/main.qml
    ui/PolicyConfigurationInterface.qml
    ui/TransmissionStatusDashboard.qml
    ui/BlockedTransmissionsViewer.qml
)

# Create executable
add_executable(milos-data-guard-gui
    ${APPLICATION_SOURCES}
    ${APPLICATION_HEADERS}
    ${QML_FILES}
)

# Link libraries
target_link_libraries(milos-data-guard-gui
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
)

# Enable Qt MOC and QRC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set_property(TARGET milos-data-guard-gui PROPERTY AUTOMOC ON)

# Create QML resource file
qt6_add_resources(milos-data-guard-gui "qml"
    PREFIX "/"
    FILES ${QML_FILES}
)

# Install executable
install(TARGETS milos-data-guard-gui
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Testing
enable_testing()
add_subdirectory(tests)

