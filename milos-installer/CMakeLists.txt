cmake_minimum_required(VERSION 3.27)
project(MilosInstaller VERSION 1.0.0 LANGUAGES CXX)

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

find_package(Qt6 REQUIRED COMPONENTS Core Quick QuickControls2 DBus Sql)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/services
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dbus
)

# Source files
set(INSTALLER_SOURCES
    src/main.cpp
    src/ui/mainwindow.cpp
    src/ui/welcomescreen.cpp
    src/ui/partitioningscreen.cpp
    src/ui/wizardnavigation.cpp
    src/services/installerstate.cpp
)

set(INSTALLER_HEADERS
    src/ui/mainwindow.h
    src/ui/welcomescreen.h
    src/ui/partitioningscreen.h
    src/ui/wizardnavigation.h
    src/services/installerstate.h
)

# QML files
set(QML_FILES
    src/ui/mainwindow.qml
    src/ui/welcomescreen.qml
    src/ui/partitioningscreen.qml
    src/ui/wizardnavigation.qml
)

# Create executable
add_executable(milos-installer
    ${INSTALLER_SOURCES}
    ${INSTALLER_HEADERS}
    ${QML_FILES}
)

target_link_libraries(milos-installer
    Qt6::Core
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
    Qt6::Sql
)

# Install executable
install(TARGETS milos-installer
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

