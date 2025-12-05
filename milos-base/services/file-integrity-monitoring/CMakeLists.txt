cmake_minimum_required(VERSION 3.27)
project(milos-file-integrity-monitoring VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

# Find Qt6 for D-Bus
find_package(Qt6 REQUIRED COMPONENTS Core DBus)

# Find SQLite3 for baseline and change storage
find_package(SQLite3 REQUIRED)

# Find yaml-cpp for YAML configuration parsing
find_package(yaml-cpp REQUIRED)

# Find CLI11 for CLI tool
find_package(CLI11 REQUIRED)

# Add logging library
add_subdirectory(../../libs/logging ${CMAKE_BINARY_DIR}/libs/logging)

# Find OpenSSL for cryptographic hashing
find_package(OpenSSL REQUIRED)

# Find libcurl for HTTP security tool adapters (optional)
find_package(CURL)
if(CURL_FOUND)
    add_definitions(-DHAVE_LIBCURL)
    message(STATUS "libcurl found: HTTP security tool adapters enabled")
else()
    message(STATUS "libcurl not found: HTTP adapters will use fallback")
endif()

# Check for inotify support (Linux)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-DHAVE_INOTIFY)
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(SERVICE_SOURCES
    src/main.cpp
    src/fim_service.cpp
    src/baseline_manager.cpp
    src/change_detector.cpp
    src/integrity_verifier.cpp
    src/file_monitor.cpp
    src/remediation_manager.cpp
    src/security_tools_integration.cpp
    src/verification_scheduler.cpp
    src/dbus_interface.cpp
    src/cli_tool.cpp
    src/config_parser.cpp
    src/audit_logger.cpp
)

set(SERVICE_HEADERS
    include/fim_service.h
    include/baseline_manager.h
    include/change_detector.h
    include/integrity_verifier.h
    include/file_monitor.h
    include/remediation_manager.h
    include/security_tools_integration.h
    include/verification_scheduler.h
    include/dbus_interface.h
    include/config_parser.h
    include/audit_logger.h
)

# Create executable
add_executable(milos-file-integrity-monitoring
    ${SERVICE_SOURCES}
    ${SERVICE_HEADERS}
)

# Link libraries
target_link_libraries(milos-file-integrity-monitoring
    Qt6::Core
    Qt6::DBus
    SQLite3::SQLite3
    yaml-cpp::yaml-cpp
    OpenSSL::SSL
    OpenSSL::Crypto
    milos-logging
)

# Link libcurl if available
if(CURL_FOUND)
    target_link_libraries(milos-file-integrity-monitoring CURL::libcurl)
endif()

# Set executable properties
set_target_properties(milos-file-integrity-monitoring
    PROPERTIES
    OUTPUT_NAME milos-file-integrity-monitoring
)

# Install executable
install(TARGETS milos-file-integrity-monitoring
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Create CLI tool executable
add_executable(milos-fim
    src/cli_tool.cpp
)

target_link_libraries(milos-fim
    Qt6::Core
    Qt6::DBus
    CLI11::CLI11
)

# Install CLI tool
install(TARGETS milos-fim
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install systemd service file
install(FILES
    data/milos-file-integrity-monitoring.service
    DESTINATION ${CMAKE_INSTALL_SYSTEMDUNITDIR}
)

# Install configuration template
install(FILES
    data/config.yaml.template
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/milos/fim
    RENAME config.yaml.template
)

# Testing
enable_testing()
add_subdirectory(tests)

