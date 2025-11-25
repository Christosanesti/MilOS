cmake_minimum_required(VERSION 3.27)
project(milos-performance-mode VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(SOURCES
    src/styling.cpp
    src/control.cpp
    src/indicators.cpp
    src/qmltypes.cpp
)

set(HEADERS
    include/styling.h
    include/control.h
    include/indicators.h
)

# Create library
add_library(milos-performance-mode STATIC
    ${SOURCES}
    ${HEADERS}
)

# Link libraries
target_link_libraries(milos-performance-mode
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
)

# Set properties
set_target_properties(milos-performance-mode
    PROPERTIES
    PUBLIC_HEADER "${HEADERS}"
)

# Install
install(TARGETS milos-performance-mode
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/milos/performance-mode
)

# Install QML files
install(FILES
    ui/ControlDialog.qml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/milos/qml/PerformanceMode
)

# Testing
enable_testing()
add_subdirectory(tests)

