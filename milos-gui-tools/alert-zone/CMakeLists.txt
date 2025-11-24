cmake_minimum_required(VERSION 3.27)
project(AlertZone VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 components
set(QT6_PREFIX_PATH "")
if(EXISTS "${CMAKE_SOURCE_DIR}/../../.local/qt6/Qt")
    set(QT6_PREFIX_PATH "${CMAKE_SOURCE_DIR}/../../.local/qt6/Qt/6.8.0/gcc_64")
endif()
if(EXISTS "$ENV{HOME}/Qt/6.8.0/gcc_64")
    set(QT6_PREFIX_PATH "$ENV{HOME}/Qt/6.8.0/gcc_64")
endif()

if(QT6_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "${QT6_PREFIX_PATH};${CMAKE_PREFIX_PATH}")
    message(STATUS "Using local Qt6 installation: ${QT6_PREFIX_PATH}")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Quick QuickControls2 DBus Qml)
find_package(KF6 REQUIRED COMPONENTS Plasma)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(ALERTZONE_SOURCES
    src/alertzoneservice.cpp
    src/alertzonewidget.cpp
    src/qmltypes.cpp
)

set(ALERTZONE_HEADERS
    src/alertzoneservice.h
    src/alertzonewidget.h
)

# Create library
add_library(alert-zone-services STATIC
    ${ALERTZONE_SOURCES}
    ${ALERTZONE_HEADERS}
)

target_link_libraries(alert-zone-services
    Qt6::Core
    Qt6::DBus
    Qt6::Qml
    KF6::Plasma
)

# Register QML types (need to include headers)
target_include_directories(alert-zone-services PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Install library
install(TARGETS alert-zone-services
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install Plasmoid package
install(DIRECTORY package/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/plasma/plasmoids/org.milos.alert-zone
    FILES_MATCHING
    PATTERN "*.qml"
    PATTERN "*.json"
)

# Install desktop file for Plasma registration
install(FILES org.milos.alert-zone.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/kservices6/plasmoid
)

