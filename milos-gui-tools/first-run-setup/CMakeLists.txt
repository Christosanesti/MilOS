cmake_minimum_required(VERSION 3.27)
project(FirstRunSetup VERSION 1.0.0 LANGUAGES CXX)

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
set(SETUP_SOURCES
    src/main.cpp
    src/ui/mainwindow.cpp
    src/ui/welcomescreen.cpp
    src/ui/securityoverview.cpp
    src/ui/firewallconfigurationscreen.cpp
    src/ui/updatecheckscreen.cpp
    src/ui/hardeningwizardscreen.cpp
    src/ui/securitypreferencesscreen.cpp
    src/ui/securitytourscreen.cpp
    src/ui/completionscreen.cpp
    src/services/setupstatemanager.cpp
    src/services/firewallservice.cpp
    src/services/updateservice.cpp
    src/services/hardeningservice.cpp
)

set(SETUP_HEADERS
    src/ui/mainwindow.h
    src/ui/welcomescreen.h
    src/ui/securityoverview.h
    src/ui/firewallconfigurationscreen.h
    src/ui/updatecheckscreen.h
    src/ui/hardeningwizardscreen.h
    src/ui/securitypreferencesscreen.h
    src/ui/securitytourscreen.h
    src/ui/completionscreen.h
    src/services/setupstatemanager.h
    src/services/firewallservice.h
    src/services/updateservice.h
    src/services/hardeningservice.h
)

# QML files
set(QML_FILES
    src/ui/mainwindow.qml
    src/ui/welcomescreen.qml
    src/ui/securityoverview.qml
    src/ui/firewallconfigurationscreen.qml
    src/ui/updatecheckscreen.qml
    src/ui/hardeningwizardscreen.qml
    src/ui/securitypreferencesscreen.qml
    src/ui/securitytourscreen.qml
    src/ui/completionscreen.qml
)

# Create executable
add_executable(first-run-setup
    ${SETUP_SOURCES}
    ${SETUP_HEADERS}
    ${QML_FILES}
)

target_link_libraries(first-run-setup
    Qt6::Core
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
    Qt6::Sql
)

# Install executable
install(TARGETS first-run-setup
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

