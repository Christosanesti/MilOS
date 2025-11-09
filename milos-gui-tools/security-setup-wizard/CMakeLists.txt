cmake_minimum_required(VERSION 3.27)
project(SecuritySetupWizard VERSION 1.0.0 LANGUAGES CXX)

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

find_package(Qt6 REQUIRED COMPONENTS Core Quick QuickControls2 DBus)

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
set(WIZARD_SOURCES
    src/main.cpp
    src/ui/mainwindow.cpp
    src/ui/hardeningwizard.cpp
    src/ui/securitypreferences.cpp
    src/ui/securitytour.cpp
    src/ui/completionscreen.cpp
    src/services/hardeningmanager.cpp
    src/services/preferencesmanager.cpp
)

set(WIZARD_HEADERS
    src/ui/mainwindow.h
    src/ui/hardeningwizard.h
    src/ui/securitypreferences.h
    src/ui/securitytour.h
    src/ui/completionscreen.h
    src/services/hardeningmanager.h
    src/services/preferencesmanager.h
)

# QML files
set(QML_FILES
    src/ui/mainwindow.qml
    src/ui/hardeningwizard.qml
    src/ui/securitypreferences.qml
    src/ui/securitytour.qml
    src/ui/completionscreen.qml
)

# Create executable
add_executable(security-setup-wizard
    ${WIZARD_SOURCES}
    ${WIZARD_HEADERS}
    ${QML_FILES}
)

target_link_libraries(security-setup-wizard
    Qt6::Core
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
)

# Install executable
install(TARGETS security-setup-wizard
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

