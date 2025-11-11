cmake_minimum_required(VERSION 3.27)
project(MilosPQCAbstraction VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

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

# Find OpenSSL for classical encryption fallback
find_package(OpenSSL REQUIRED)

# Find yaml-cpp for YAML configuration parsing
find_package(yaml-cpp REQUIRED)

# Find CLI11 for CLI tool
find_package(CLI11 REQUIRED)

# Note: liboqs will be integrated in Story 17.2
# For now, we'll prepare the structure for liboqs integration
pkg_check_modules(LIBOQS QUIET liboqs)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files for shared library
set(LIBRARY_SOURCES
    src/algorithm_interface.cpp
    src/algorithm_factory.cpp
    src/algorithm_registry.cpp
    src/hybrid_encryption.cpp
    src/config_parser.cpp
    src/algorithm_versioning.cpp
    src/fallback_manager.cpp
)

set(LIBRARY_HEADERS
    include/milos/pqc/algorithm_interface.h
    include/milos/pqc/algorithm_factory.h
    include/milos/pqc/algorithm_registry.h
    include/milos/pqc/hybrid_encryption.h
    include/milos/pqc/config_parser.h
    include/milos/pqc/algorithm_versioning.h
    include/milos/pqc/fallback_manager.h
)

# Create shared library
add_library(milos-pqc SHARED
    ${LIBRARY_SOURCES}
    ${LIBRARY_HEADERS}
)

# Link libraries
target_link_libraries(milos-pqc
    OpenSSL::SSL
    OpenSSL::Crypto
    yaml-cpp::yaml-cpp
)

if(USE_QT_DBUS)
    set(CMAKE_AUTOMOC ON)
    target_link_libraries(milos-pqc
        Qt6::Core
        Qt6::DBus
    )
    set_property(TARGET milos-pqc PROPERTY AUTOMOC ON)
else()
    target_link_libraries(milos-pqc
        ${DBUS_LIBRARIES}
    )
    target_include_directories(milos-pqc PRIVATE ${DBUS_INCLUDE_DIRS})
endif()

# Link liboqs if available
if(LIBOQS_FOUND)
    target_link_libraries(milos-pqc ${LIBOQS_LIBRARIES})
    target_include_directories(milos-pqc PRIVATE ${LIBOQS_INCLUDE_DIRS})
endif()

# Set library properties
set_target_properties(milos-pqc PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${LIBRARY_HEADERS}"
)

# Install library
install(TARGETS milos-pqc
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/milos/pqc
)

# Install configuration template
install(FILES data/config.yaml.template
    DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/milos/pqc
    RENAME config.yaml
)

# Create D-Bus service executable (optional, for D-Bus interface)
add_executable(milos-pqc-service
    src/dbus_service.cpp
    src/dbus_interface.cpp
)

target_link_libraries(milos-pqc-service
    milos-pqc
    CLI11::CLI11
)

if(USE_QT_DBUS)
    target_link_libraries(milos-pqc-service
        Qt6::Core
        Qt6::DBus
    )
    set_property(TARGET milos-pqc-service PROPERTY AUTOMOC ON)
endif()

install(TARGETS milos-pqc-service
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Create CLI tool executable
add_executable(milos-pqc
    src/cli_tool.cpp
)

target_link_libraries(milos-pqc
    milos-pqc
    CLI11::CLI11
)

if(USE_QT_DBUS)
    target_link_libraries(milos-pqc
        Qt6::Core
        Qt6::DBus
    )
    set_property(TARGET milos-pqc PROPERTY AUTOMOC ON)
endif()

install(TARGETS milos-pqc
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Testing
enable_testing()
add_subdirectory(tests)

