cmake_minimum_required(VERSION 3.16)
project(stig-compliance-checker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core DBus)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/milos-base/services/stig-compliance-checker/include
    ${CMAKE_SOURCE_DIR}/milos-base/libs/logging/include
)

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

