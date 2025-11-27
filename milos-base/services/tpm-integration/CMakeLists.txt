cmake_minimum_required(VERSION 3.27)
project(milos-tpm-integration VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

# Find Qt6 for D-Bus
find_package(Qt6 REQUIRED COMPONENTS Core DBus)

# Find TSS2 libraries (TPM Software Stack 2.0)
pkg_check_modules(TSS2 tss2-esys tss2-mu tss2-tctildr)
if(NOT TSS2_FOUND)
    message(WARNING "TSS2 libraries not found. TPM integration will not be available.")
    message(STATUS "Install tpm2-tss to enable TPM 2.0 support.")
    add_definitions(-DHAVE_TSS2=0)
else()
    add_definitions(-DHAVE_TSS2=1)
    message(STATUS "TSS2 libraries found: ${TSS2_LIBRARIES}")
endif()

# Find CLI11 for CLI tool
find_package(CLI11 REQUIRED)

# Find OpenSSL for cryptographic operations
find_package(OpenSSL REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(SERVICE_SOURCES
    src/main.cpp
    src/tpm_service.cpp
    src/tpm_device.cpp
    src/tpm_key_manager.cpp
    src/tpm_attestation.cpp
    src/tpm_health_monitor.cpp
    src/secure_boot_manager.cpp
    src/dbus_interface.cpp
    src/cli_tool.cpp
)

set(SERVICE_HEADERS
    include/tpm_service.h
    include/tpm_device.h
    include/tpm_key_manager.h
    include/tpm_attestation.h
    include/tpm_health_monitor.h
    include/secure_boot_manager.h
    include/dbus_interface.h
)

# Create executable
add_executable(milos-tpm-integration
    ${SERVICE_SOURCES}
    ${SERVICE_HEADERS}
)

# Link logging library
add_subdirectory(../../libs/logging ${CMAKE_BINARY_DIR}/libs/logging)

# Link libraries
target_link_libraries(milos-tpm-integration
    Qt6::Core
    Qt6::DBus
    OpenSSL::SSL
    OpenSSL::Crypto
    milos::logging
)

if(TSS2_FOUND)
    target_link_libraries(milos-tpm-integration ${TSS2_LIBRARIES})
    target_include_directories(milos-tpm-integration PRIVATE ${TSS2_INCLUDE_DIRS})
    target_compile_definitions(milos-tpm-integration PRIVATE HAVE_TSS2)
endif()

# Set executable properties
set_target_properties(milos-tpm-integration
    PROPERTIES
    OUTPUT_NAME milos-tpm-integration
)

# Install executable
install(TARGETS milos-tpm-integration
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Create CLI tool executable
add_executable(milos-tpm
    src/cli_tool.cpp
)

target_link_libraries(milos-tpm
    Qt6::Core
    Qt6::DBus
    CLI11::CLI11
)

# Install CLI tool
install(TARGETS milos-tpm
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install systemd service file
install(FILES
    data/milos-tpm-integration.service
    DESTINATION ${CMAKE_INSTALL_SYSTEMDUNITDIR}
)

# Testing
enable_testing()
add_subdirectory(tests)

