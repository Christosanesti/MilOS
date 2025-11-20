cmake_minimum_required(VERSION 3.27)
project(MilosSecureMessenger VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Quick QuickControls2 DBus)

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
    src/secure_messenger.cpp
    src/usb_authorization.cpp
    src/key_generator.cpp
    src/key_manager.cpp
    src/user_enrollment.cpp
    src/role_manager.cpp
    src/dbus_interface.cpp
    src/audit_logger.cpp
)

# Header files
set(APPLICATION_HEADERS
    include/secure_messenger.h
    include/usb_authorization.h
    include/key_generator.h
    include/key_manager.h
    include/user_enrollment.h
    include/role_manager.h
    include/dbus_interface.h
    include/audit_logger.h
)

# QML files
set(QML_FILES
    ui/main.qml
    ui/key_management.qml
)

# Create executable
add_executable(milos-secure-messenger
    ${APPLICATION_SOURCES}
    ${APPLICATION_HEADERS}
)

# Enable MOC for Q_OBJECT classes
set_property(TARGET milos-secure-messenger PROPERTY AUTOMOC ON)

# Link libraries
target_link_libraries(milos-secure-messenger
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
    CLI11::CLI11
)

# Add QML resources
qt_add_qml_module(milos-secure-messenger
    URI milos.secure.messenger
    VERSION 1.0
    QML_FILES ${QML_FILES}
)

# CLI tool executable
add_executable(milos-messenger
    src/cli_tool.cpp
)

target_link_libraries(milos-messenger
    Qt6::Core
    Qt6::DBus
    CLI11::CLI11
)

# Install targets
install(TARGETS milos-secure-messenger milos-messenger
    RUNTIME DESTINATION bin
)

