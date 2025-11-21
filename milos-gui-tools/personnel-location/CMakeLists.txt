cmake_minimum_required(VERSION 3.27)
project(MilosPersonnelLocation VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Quick QuickControls2 DBus Location Positioning Sql)

# Find SQLite3
find_package(SQLite3 REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(APPLICATION_SOURCES
    src/main.cpp
    src/personnel_location_system.cpp
    src/location_tracker.cpp
    src/location_storage.cpp
    src/location_encryption.cpp
    src/dbus_interface.cpp
    src/audit_logger.cpp
)

# Header files
set(APPLICATION_HEADERS
    include/personnel_location_system.h
    include/location_tracker.h
    include/location_storage.h
    include/location_encryption.h
    include/dbus_interface.h
    include/audit_logger.h
)

# QML files
set(QML_FILES
    ui/main.qml
)

# Create executable
add_executable(milos-personnel-location
    ${APPLICATION_SOURCES}
    ${APPLICATION_HEADERS}
)

# Enable MOC for Q_OBJECT classes
set_property(TARGET milos-personnel-location PROPERTY AUTOMOC ON)

# Link libraries
target_link_libraries(milos-personnel-location
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
    Qt6::Location
    Qt6::Positioning
    Qt6::Sql
    SQLite::SQLite3
)

# Add QML resources
qt_add_qml_module(milos-personnel-location
    URI milos.personnellocation
    VERSION 1.0
    QML_FILES ${QML_FILES}
)

# Install targets
install(TARGETS milos-personnel-location
    RUNTIME DESTINATION bin
)

