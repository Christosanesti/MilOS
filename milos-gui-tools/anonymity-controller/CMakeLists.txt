cmake_minimum_required(VERSION 3.27)
project(milos-anonymity-controller VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick DBus Gui)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(SOURCES
    src/anonymitymanager.cpp
    src/qmltypes.cpp
)

set(HEADERS
    include/anonymitymanager.h
)

# Create library
add_library(milos-anonymity-controller STATIC
    ${SOURCES}
    ${HEADERS}
)

# Link libraries
target_link_libraries(milos-anonymity-controller
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    Qt6::DBus
)

# Create executable
add_executable(milos-anonymity-controller-app
    src/main.cpp
)

target_link_libraries(milos-anonymity-controller-app
    milos-anonymity-controller
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    Qt6::Gui
)

# Add QML files as resources
qt_add_qml_module(milos-anonymity-controller-app
    URI AnonymityController
    VERSION 1.0
    QML_FILES
        ui/main.qml
)

# Set properties
set_target_properties(milos-anonymity-controller
    PROPERTIES
    PUBLIC_HEADER "${HEADERS}"
)

# Install
install(TARGETS milos-anonymity-controller milos-anonymity-controller-app
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/milos/anonymity-controller
)

# Install QML files
install(FILES
    ui/main.qml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/milos/qml/AnonymityController
)

