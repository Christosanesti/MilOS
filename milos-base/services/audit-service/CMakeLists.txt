cmake_minimum_required(VERSION 3.27)
project(MilosAuditService VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(systemd REQUIRED)

# Find D-Bus (using Qt D-Bus or libdbus)
find_package(Qt6 QUIET COMPONENTS Core DBus Sql)
if(Qt6_FOUND)
    set(USE_QT_DBUS ON)
    message(STATUS "Using Qt6 D-Bus")
else()
    pkg_check_modules(DBUS REQUIRED dbus-1)
    set(USE_QT_DBUS OFF)
    message(STATUS "Using libdbus")
endif()

# Find SQLite
find_package(SQLite3 REQUIRED)

# Find OpenSSL for cryptographic hashing
find_package(OpenSSL REQUIRED)

# Find yaml-cpp for YAML configuration parsing
find_package(yaml-cpp REQUIRED)

# Find CLI11 for CLI tool
find_package(CLI11 REQUIRED)

# Add logging library
add_subdirectory(../../libs/logging ${CMAKE_BINARY_DIR}/libs/logging)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(SERVICE_SOURCES
    src/main.cpp
    src/audit_service.cpp
    src/log_storage.cpp
    src/hash_chain.cpp
    src/event_collector.cpp
    src/query_engine.cpp
    src/dbus_interface.cpp
    src/socket_interface.cpp
    src/config_parser.cpp
)

set(SERVICE_HEADERS
    src/audit_service.h
    src/log_storage.h
    src/hash_chain.h
    src/event_collector.h
    src/query_engine.h
    src/dbus_interface.h
    src/socket_interface.h
    src/config_parser.h
)

# Create service daemon executable
add_executable(milos-audit-service-daemon
    ${SERVICE_SOURCES}
    ${SERVICE_HEADERS}
)

# Link libraries
target_link_libraries(milos-audit-service-daemon
    SQLite::SQLite3
    OpenSSL::SSL
    OpenSSL::Crypto
    yaml-cpp::yaml-cpp
    milos-logging
)

if(USE_QT_DBUS)
    set(CMAKE_AUTOMOC ON)
    target_link_libraries(milos-audit-service-daemon
        Qt6::Core
        Qt6::DBus
        Qt6::Sql
    )
    set_property(TARGET milos-audit-service-daemon PROPERTY AUTOMOC ON)
else()
    target_link_libraries(milos-audit-service-daemon
        ${DBUS_LIBRARIES}
    )
    target_include_directories(milos-audit-service-daemon PRIVATE ${DBUS_INCLUDE_DIRS})
endif()

# Create CLI tool executable
add_executable(milos-audit
    src/cli_tool.cpp
)

target_link_libraries(milos-audit
    CLI11::CLI11
    SQLite::SQLite3
    OpenSSL::SSL
    OpenSSL::Crypto
    yaml-cpp::yaml-cpp
)

if(USE_QT_DBUS)
    set(CMAKE_AUTOMOC ON)
    target_link_libraries(milos-audit
        Qt6::Core
        Qt6::DBus
        Qt6::Sql
    )
    set_property(TARGET milos-audit PROPERTY AUTOMOC ON)
else()
    target_link_libraries(milos-audit
        ${DBUS_LIBRARIES}
    )
    target_include_directories(milos-audit PRIVATE ${DBUS_INCLUDE_DIRS})
endif()

# Install executables
install(TARGETS milos-audit-service-daemon milos-audit
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install systemd service file
install(FILES data/milos-audit-service.service
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/systemd/system
)

# Install configuration template
install(FILES data/config.yaml.template
    DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/milos/audit-service
    RENAME config.yaml
)

# Install configuration directory structure
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/milos/audit-service
    FILES_MATCHING PATTERN "*.yaml"
)

# Testing
enable_testing()
add_subdirectory(tests)

