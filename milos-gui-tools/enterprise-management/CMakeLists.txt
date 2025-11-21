cmake_minimum_required(VERSION 3.27)
project(MilosEnterpriseManagement VERSION 1.0.0 LANGUAGES CXX)

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
    src/enterprise_management_system.cpp
    src/config_manager.cpp
    src/deployment_orchestrator.cpp
    src/multi_site_manager.cpp
    src/dbus_interface.cpp
    src/audit_logger.cpp
)

# Header files
set(APPLICATION_HEADERS
    include/enterprise_management_system.h
    include/config_manager.h
    include/deployment_orchestrator.h
    include/multi_site_manager.h
    include/dbus_interface.h
    include/audit_logger.h
)

# QML files
set(QML_FILES
    ui/main.qml
)

# Create executable
add_executable(milos-enterprise-management
    ${APPLICATION_SOURCES}
    ${APPLICATION_HEADERS}
)

# Enable MOC for Q_OBJECT classes
set_property(TARGET milos-enterprise-management PROPERTY AUTOMOC ON)

# Link libraries
target_link_libraries(milos-enterprise-management
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
)

# Add QML resources
qt_add_qml_module(milos-enterprise-management
    URI milos.enterprisemanagement
    VERSION 1.0
    QML_FILES ${QML_FILES}
)

# Install targets
install(TARGETS milos-enterprise-management
    RUNTIME DESTINATION bin
)

