cmake_minimum_required(VERSION 3.27)
project(EmergencyWidget VERSION 1.0.0 LANGUAGES CXX)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/services
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)

# Source files
set(EMERGENCY_SOURCES
    src/services/networkkillswitch.cpp
    src/services/screenlock.cpp
    src/services/securemode.cpp
    src/services/emergencyshutdown.cpp
    src/services/auditlogger.cpp
    src/emergencywidget.cpp
    src/widgetconfig.cpp
    src/keyboardshortcut.cpp
    src/statusindicators.cpp
)

set(EMERGENCY_HEADERS
    src/services/networkkillswitch.h
    src/services/screenlock.h
    src/services/securemode.h
    src/services/emergencyshutdown.h
    src/services/auditlogger.h
    src/emergencywidget.h
    src/widgetconfig.h
    src/keyboardshortcut.h
    src/statusindicators.h
)

# Create library
add_library(emergency-widget-services STATIC
    ${EMERGENCY_SOURCES}
    ${EMERGENCY_HEADERS}
)

target_link_libraries(emergency-widget-services
    Qt6::Core
    Qt6::DBus
    Qt6::Qml
    KF6::Plasma
)

# Install library
install(TARGETS emergency-widget-services
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install Plasmoid package
install(DIRECTORY package/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/plasma/plasmoids/org.milos.emergency-widget
    FILES_MATCHING
    PATTERN "*.qml"
    PATTERN "*.json"
    PATTERN "*.desktop"
)

# Install desktop file for Plasma registration
install(FILES org.milos.emergency-widget.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/kservices6/plasmoid
)
