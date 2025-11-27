cmake_minimum_required(VERSION 3.27)
project(milos-secrets-management VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

# Find Qt6 for D-Bus
find_package(Qt6 REQUIRED COMPONENTS Core DBus)

# Find SQLite3 for secret storage
find_package(SQLite3 REQUIRED)

# Find yaml-cpp for YAML configuration parsing
find_package(yaml-cpp REQUIRED)

# Find CLI11 for CLI tool
find_package(CLI11 REQUIRED)

# Find OpenSSL for encryption
find_package(OpenSSL REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(SERVICE_SOURCES
    src/main.cpp
    src/secrets_service.cpp
    src/secret_storage.cpp
    src/secret_rotation.cpp
    src/secret_versioning.cpp
    src/access_control.cpp
    src/application_integration.cpp
    src/dbus_interface.cpp
    src/cli_tool.cpp
    src/config_parser.cpp
    src/audit_logger.cpp
)

set(SERVICE_HEADERS
    include/secrets_service.h
    include/secret_storage.h
    include/secret_rotation.h
    include/secret_versioning.h
    include/access_control.h
    include/application_integration.h
    include/dbus_interface.h
    include/config_parser.h
    include/audit_logger.h
)

# Create executable
add_executable(milos-secrets-management
    ${SERVICE_SOURCES}
    ${SERVICE_HEADERS}
)

# Link logging library
add_subdirectory(../../libs/logging ${CMAKE_BINARY_DIR}/libs/logging)

# Link libraries
target_link_libraries(milos-secrets-management
    Qt6::Core
    Qt6::DBus
    SQLite3::SQLite3
    yaml-cpp::yaml-cpp
    OpenSSL::SSL
    OpenSSL::Crypto
    CLI11::CLI11
    milos::logging
)

# Set executable properties
set_target_properties(milos-secrets-management
    PROPERTIES
    OUTPUT_NAME milos-secrets-management
)

# Install executable
install(TARGETS milos-secrets-management
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install systemd service file
install(FILES
    data/milos-secrets-management.service
    DESTINATION ${CMAKE_INSTALL_SYSTEMDUNITDIR}
)

# Install configuration template
install(FILES
    data/config.yaml.template
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/milos/secrets
    RENAME config.yaml.template
)

# Testing
enable_testing()
add_subdirectory(tests)

