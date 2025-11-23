cmake_minimum_required(VERSION 3.27)
project(MilosUpdateGui VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Quick QuickControls2 DBus)

# Find SQLite3 for update history storage (if needed)
find_package(SQLite3 REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(APPLICATION_SOURCES
    src/main.cpp
    src/update_manager.cpp
    src/dbus_interface_wrapper.cpp
)

set(APPLICATION_HEADERS
    include/update_manager.h
    include/dbus_interface_wrapper.h
)

# QML files
set(QML_FILES
    ui/main.qml
    ui/UpdateCheckInterface.qml
    ui/UpdateInstallationInterface.qml
    ui/UpdateHistoryViewer.qml
    ui/RollbackInterface.qml
    ui/ServiceStatusDashboard.qml
)

# Create executable
add_executable(milos-update-gui
    ${APPLICATION_SOURCES}
    ${APPLICATION_HEADERS}
    ${QML_FILES}
)

# Link libraries
target_link_libraries(milos-update-gui
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
    SQLite3::SQLite3
)

# Enable Qt MOC and QRC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set_property(TARGET milos-update-gui PROPERTY AUTOMOC ON)

# Create QML resource file
qt6_add_resources(milos-update-gui "qml"
    PREFIX "/"
    FILES ${QML_FILES}
)

# Install executable
install(TARGETS milos-update-gui
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Testing
enable_testing()
add_subdirectory(tests)

