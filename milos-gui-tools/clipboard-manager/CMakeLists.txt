cmake_minimum_required(VERSION 3.27)
project(MilosClipboardManager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Quick QuickControls2 DBus)

# Find PQC encryption library (from Epic 17)
find_package(milos-pqc REQUIRED)

# Find yaml-cpp for YAML configuration
find_package(yaml-cpp REQUIRED)

# Find CLI11 for CLI tool
find_package(CLI11 REQUIRED)

# Find SQLite3 for clipboard history
find_package(SQLite3 REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(APPLICATION_SOURCES
    src/main.cpp
    src/clipboard_manager.cpp
    src/clipboard_service.cpp
    src/clipboard_encryption.cpp
    src/clipboard_history.cpp
    src/clipboard_integration.cpp
    src/dbus_interface.cpp
    src/config_parser.cpp
    src/audit_logger.cpp
)

set(APPLICATION_HEADERS
    include/clipboard_manager.h
    include/clipboard_service.h
    include/clipboard_encryption.h
    include/clipboard_history.h
    include/clipboard_integration.h
    include/dbus_interface.h
    include/config_parser.h
    include/audit_logger.h
)

# QML files
set(QML_FILES
    ui/main.qml
    ui/clipboard_view.qml
    ui/clipboard_history.qml
    ui/status_indicator.qml
)

# Create executable
add_executable(milos-clipboard-manager
    ${APPLICATION_SOURCES}
    ${APPLICATION_HEADERS}
    ${QML_FILES}
)

# Link libraries
target_link_libraries(milos-clipboard-manager
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
    milos-pqc
    yaml-cpp::yaml-cpp
    CLI11::CLI11
    SQLite3::SQLite3
)

# Enable Qt MOC and QRC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set_property(TARGET milos-clipboard-manager PROPERTY AUTOMOC ON)

# Create QML resource file
qt6_add_resources(milos-clipboard-manager "qml"
    PREFIX "/"
    FILES ${QML_FILES}
)

# Install executable
install(TARGETS milos-clipboard-manager
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install configuration template
install(FILES data/config.yaml.template
    DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/milos/clipboard-manager
    RENAME config.yaml
)

# Create CLI tool executable
add_executable(milos-clipboard
    src/cli_tool.cpp
)

target_link_libraries(milos-clipboard
    Qt6::Core
    Qt6::DBus
    CLI11::CLI11
)

set_property(TARGET milos-clipboard PROPERTY AUTOMOC ON)

install(TARGETS milos-clipboard
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Testing
enable_testing()
add_subdirectory(tests)

