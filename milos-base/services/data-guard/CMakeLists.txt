cmake_minimum_required(VERSION 3.27)
project(MilosDataGuard VERSION 1.0.0 LANGUAGES CXX)

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

# Find libpcap for network inspection
pkg_check_modules(LIBPCAP REQUIRED libpcap)

# Find yaml-cpp for YAML configuration parsing
find_package(yaml-cpp REQUIRED)

# Find CLI11 for CLI tool
find_package(CLI11 REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(SERVICE_SOURCES
    src/main.cpp
    src/dataguard_service.cpp
    src/network_enforcement.cpp
    src/policy_manager.cpp
    src/dbus_interface.cpp
    src/config_parser.cpp
    src/audit_logger.cpp
)

set(SERVICE_HEADERS
    src/dataguard_service.h
    src/network_enforcement.h
    src/policy_manager.h
    src/dbus_interface.h
    src/config_parser.h
    src/audit_logger.h
)

# Create service daemon executable
add_executable(milos-data-guard-daemon
    ${SERVICE_SOURCES}
    ${SERVICE_HEADERS}
)

# Link libraries
target_link_libraries(milos-data-guard-daemon
    ${LIBPCAP_LIBRARIES}
    yaml-cpp::yaml-cpp
    pthread
)

if(USE_QT_DBUS)
    set(CMAKE_AUTOMOC ON)
    target_link_libraries(milos-data-guard-daemon
        Qt6::Core
        Qt6::DBus
    )
    # Enable MOC for Q_OBJECT classes
    set_property(TARGET milos-data-guard-daemon PROPERTY AUTOMOC ON)
    # Add Qt JSON support
    find_package(Qt6 REQUIRED COMPONENTS Core DBus)
    target_link_libraries(milos-data-guard-daemon Qt6::Core)
else()
    target_link_libraries(milos-data-guard-daemon
        ${DBUS_LIBRARIES}
    )
    target_include_directories(milos-data-guard-daemon PRIVATE ${DBUS_INCLUDE_DIRS})
endif()

target_include_directories(milos-data-guard-daemon PRIVATE ${LIBPCAP_INCLUDE_DIRS})

# Create CLI tool executable
add_executable(milos-data-guard
    src/cli_tool.cpp
)

target_link_libraries(milos-data-guard
    CLI11::CLI11
    pthread
)

if(USE_QT_DBUS)
    set(CMAKE_AUTOMOC ON)
    target_link_libraries(milos-data-guard
        Qt6::Core
        Qt6::DBus
    )
    set_property(TARGET milos-data-guard PROPERTY AUTOMOC ON)
else()
    target_link_libraries(milos-data-guard
        ${DBUS_LIBRARIES}
    )
    target_include_directories(milos-data-guard PRIVATE ${DBUS_INCLUDE_DIRS})
endif()

# Install executables
install(TARGETS milos-data-guard-daemon milos-data-guard
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install systemd service file
install(FILES data/milos-data-guard.service
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/systemd/system
)

# Install configuration template
install(FILES data/config.yaml.template
    DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/milos/data-guard
    RENAME config.yaml
)

# Install configuration directory structure
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/milos/data-guard
    FILES_MATCHING PATTERN "*.yaml"
)

