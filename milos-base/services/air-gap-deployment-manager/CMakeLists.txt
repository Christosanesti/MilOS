cmake_minimum_required(VERSION 3.27)
project(MilosAirGapDeploymentManager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core)

# Add logging library
add_subdirectory(../../libs/logging ${CMAKE_BINARY_DIR}/libs/logging)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(SERVICE_SOURCES
    src/air_gap_deployment_manager.cpp
)

set(SERVICE_HEADERS
    include/air_gap_deployment_manager.h
)

# Create library
add_library(milos-air-gap-deployment-manager
    ${SERVICE_SOURCES}
    ${SERVICE_HEADERS}
)

# Create CLI tool executable
add_executable(milos-air-gap-package
    src/cli_tool.cpp
)

# Link libraries
target_link_libraries(milos-air-gap-deployment-manager
    Qt6::Core
    milos-logging
)

target_link_libraries(milos-air-gap-package
    milos-air-gap-deployment-manager
    Qt6::Core
    milos-logging
)

# Enable MOC for Q_OBJECT classes
set_property(TARGET milos-air-gap-deployment-manager PROPERTY AUTOMOC ON)

# Install library and CLI tool
install(TARGETS milos-air-gap-deployment-manager milos-air-gap-package
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(FILES include/air_gap_deployment_manager.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/milos/air-gap-deployment-manager
)

