cmake_minimum_required(VERSION 3.27)
project(milos-navigation-reset-ui VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick DBus)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../navigation/src
)

# Source files
set(SOURCES
    src/resetui.cpp
    src/qmltypes.cpp
)

set(HEADERS
    src/resetui.h
)

# Create library
add_library(milos-navigation-reset-ui STATIC
    ${SOURCES}
    ${HEADERS}
)

# Link libraries
target_link_libraries(milos-navigation-reset-ui
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    Qt6::DBus
)

# Set properties
set_target_properties(milos-navigation-reset-ui
    PROPERTIES
    PUBLIC_HEADER "${HEADERS}"
)

# Install
install(TARGETS milos-navigation-reset-ui
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/milos/navigation-reset
)

# Install QML files
install(FILES
    ui/ResetDialog.qml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/milos/qml/NavigationReset
)

