cmake_minimum_required(VERSION 3.27)
project(EmergencyWidget VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find KDE Frameworks 6 and Qt6 components
find_package(KF6 REQUIRED COMPONENTS 
    CoreAddons
    Plasma
    Config
    I18n
    Package
)

find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Quick 
    QuickControls2
    DBus
)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files for C++ backend
set(SOURCES
    src/networkkillswitch.cpp
    src/emergencywidgetplugin.cpp
)

set(HEADERS
    src/networkkillswitch.h
)

# Create shared library for QML plugin
add_library(emergencywidgetplugin SHARED ${SOURCES} ${HEADERS})

# Link Qt6 libraries
target_link_libraries(emergencywidgetplugin
    Qt6::Core
    Qt6::Qml
    Qt6::DBus
    Qt6::Quick
)

# Install library
install(TARGETS emergencywidgetplugin
    LIBRARY DESTINATION ${KDE_INSTALL_QMLDIR}/org/milos/emergencywidget
)

# Install qmldir file
install(FILES src/qmldir
    DESTINATION ${KDE_INSTALL_QMLDIR}/org/milos/emergencywidget
)

# Install Plasmoid package directory structure
install(DIRECTORY package/
    DESTINATION ${KDE_INSTALL_PLASMAPACKAGEDIR}/org.milos.emergency-widget
    FILES_MATCHING 
    PATTERN "*.qml"
    PATTERN "*.js"
    PATTERN "*.json"
    PATTERN "*.desktop"
    PATTERN "*.svg"
    PATTERN "*.png"
)

# Install desktop file for Plasmoid
install(FILES
    org.milos.emergency-widget.desktop
    DESTINATION ${KDE_INSTALL_KSERVICES5DIR}
)

