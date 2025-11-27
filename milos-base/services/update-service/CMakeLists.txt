cmake_minimum_required(VERSION 3.27)
project(MilosUpdateService VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(systemd REQUIRED)

# Find D-Bus (using Qt D-Bus or libdbus)
find_package(Qt6 QUIET COMPONENTS Core DBus)
if(Qt6_FOUND)
    set(USE_QT_DBUS ON)
    message(STATUS "Using Qt6 D-Bus")
else()
    pkg_check_modules(DBUS REQUIRED dbus-1)
    set(USE_QT_DBUS OFF)
    message(STATUS "Using libdbus")
endif()

# Find GPG for signature verification
pkg_check_modules(GPGME REQUIRED gpgme)

# Find SQLite for update history
find_package(SQLite3 REQUIRED)

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
    src/update_service.cpp
    src/package_manager.cpp
    src/signature_verifier.cpp
    src/update_applier.cpp
    src/rollback_manager.cpp
    src/dbus_interface.cpp
    src/config_parser.cpp
    src/audit_logger.cpp
)

set(SERVICE_HEADERS
    src/update_service.h
    src/package_manager.h
    src/signature_verifier.h
    src/update_applier.h
    src/rollback_manager.h
    src/dbus_interface.h
    src/config_parser.h
    src/audit_logger.h
)

# Create service daemon executable
add_executable(milos-update-service-daemon
    ${SERVICE_SOURCES}
    ${SERVICE_HEADERS}
)

# Link libraries
target_link_libraries(milos-update-service-daemon
    ${GPGME_LIBRARIES}
    SQLite::SQLite3
    yaml-cpp::yaml-cpp
    milos-logging
)

target_include_directories(milos-update-service-daemon PRIVATE ${GPGME_INCLUDE_DIRS})

if(USE_QT_DBUS)
    set(CMAKE_AUTOMOC ON)
    target_link_libraries(milos-update-service-daemon
        Qt6::Core
        Qt6::DBus
    )
    set_property(TARGET milos-update-service-daemon PROPERTY AUTOMOC ON)
else()
    target_link_libraries(milos-update-service-daemon
        ${DBUS_LIBRARIES}
    )
    target_include_directories(milos-update-service-daemon PRIVATE ${DBUS_INCLUDE_DIRS})
endif()

# Create CLI tool executable
add_executable(milos-update
    src/cli_tool.cpp
)

target_link_libraries(milos-update
    CLI11::CLI11
    SQLite::SQLite3
    yaml-cpp::yaml-cpp
)

if(USE_QT_DBUS)
    set(CMAKE_AUTOMOC ON)
    target_link_libraries(milos-update
        Qt6::Core
        Qt6::DBus
    )
    set_property(TARGET milos-update PROPERTY AUTOMOC ON)
else()
    target_link_libraries(milos-update
        ${DBUS_LIBRARIES}
    )
    target_include_directories(milos-update PRIVATE ${DBUS_INCLUDE_DIRS})
endif()

# Install executables
install(TARGETS milos-update-service-daemon milos-update
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install systemd service file
install(FILES data/milos-update-service.service
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/systemd/system
)

# Install configuration template
install(FILES data/config.yaml.template
    DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/milos/update-service
    RENAME config.yaml
)

# Install configuration directory structure
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/milos/update-service
    FILES_MATCHING PATTERN "*.yaml"
)

# Testing
enable_testing()
add_subdirectory(tests)

