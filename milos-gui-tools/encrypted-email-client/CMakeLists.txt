cmake_minimum_required(VERSION 3.27)
project(MilosEncryptedEmail VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Quick QuickControls2 DBus Sql)

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
    src/encrypted_email_system.cpp
    src/email_client.cpp
    src/email_storage.cpp
    src/dbus_interface.cpp
    src/audit_logger.cpp
)

# Header files
set(APPLICATION_HEADERS
    include/encrypted_email_system.h
    include/email_client.h
    include/email_storage.h
    include/dbus_interface.h
    include/audit_logger.h
)

# QML files
set(QML_FILES
    ui/main.qml
)

# Create executable
add_executable(milos-encrypted-email
    ${APPLICATION_SOURCES}
    ${APPLICATION_HEADERS}
)

# Enable MOC for Q_OBJECT classes
set_property(TARGET milos-encrypted-email PROPERTY AUTOMOC ON)

# Link libraries
target_link_libraries(milos-encrypted-email
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
    Qt6::Sql
    SQLite::SQLite3
)

# Add QML resources
qt_add_qml_module(milos-encrypted-email
    URI milos.encryptedemail
    VERSION 1.0
    QML_FILES ${QML_FILES}
)

# Install targets
install(TARGETS milos-encrypted-email
    RUNTIME DESTINATION bin
)

