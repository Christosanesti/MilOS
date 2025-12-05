cmake_minimum_required(VERSION 3.16)
project(stig-compliance-checker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core DBus)

# Enable MOC for Q_OBJECT classes
set(CMAKE_AUTOMOC ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/logging/include
)

# Try to find or build logging library
set(LOGGING_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../libs/logging")
if(NOT TARGET milos-logging)
    # Check if logging library is already built
    find_library(LOGGING_LIB milos-logging PATHS "${LOGGING_DIR}/build" NO_DEFAULT_PATH)
    if(LOGGING_LIB)
        message(STATUS "Found pre-built milos-logging: ${LOGGING_LIB}")
        add_library(milos-logging STATIC IMPORTED)
        set_target_properties(milos-logging PROPERTIES IMPORTED_LOCATION ${LOGGING_LIB})
    else()
        # Build logging library
        add_subdirectory(${LOGGING_DIR} ${CMAKE_BINARY_DIR}/libs/logging)
    endif()
endif()

# Source files
set(STIG_CHECKER_SOURCES
    src/stig_compliance_checker.cpp
)

set(STIG_CHECKER_HEADERS
    include/stig_compliance_checker.h
)

# Create library
add_library(milos-stig-compliance-checker STATIC
    ${STIG_CHECKER_SOURCES}
    ${STIG_CHECKER_HEADERS}
)

target_link_libraries(milos-stig-compliance-checker
    Qt6::Core
    Qt6::DBus
    milos-logging
)

# CLI tool
add_executable(milos-stig-checker
    src/cli_tool.cpp
)

target_link_libraries(milos-stig-checker
    milos-stig-compliance-checker
    Qt6::Core
    Qt6::DBus
    milos-logging
)

# Install
install(TARGETS milos-stig-checker
    RUNTIME DESTINATION bin
)

install(FILES ${STIG_CHECKER_HEADERS}
    DESTINATION include/milos/stig-compliance-checker
)

