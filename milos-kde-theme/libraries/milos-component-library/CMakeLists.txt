cmake_minimum_required(VERSION 3.27)
project(MilosComponents VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 components
set(QT6_PREFIX_PATH "")
if(EXISTS "${CMAKE_SOURCE_DIR}/../../../.local/qt6/Qt")
    set(QT6_PREFIX_PATH "${CMAKE_SOURCE_DIR}/../../../.local/qt6/Qt/6.8.0/gcc_64")
endif()
if(EXISTS "$ENV{HOME}/Qt/6.8.0/gcc_64")
    set(QT6_PREFIX_PATH "$ENV{HOME}/Qt/6.8.0/gcc_64")
endif()

if(QT6_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "${QT6_PREFIX_PATH};${CMAKE_PREFIX_PATH}")
    message(STATUS "Using local Qt6 installation: ${QT6_PREFIX_PATH}")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Quick QuickControls2)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/components
)

# Component source files
set(COMPONENT_SOURCES
    src/components/Button.cpp
    src/components/InputField.cpp
    src/components/StatusIndicator.cpp
    src/components/ProgressBar.cpp
    src/components/TabNavigation.cpp
    src/components/NotificationAlert.cpp
    src/components/Card.cpp
    src/qml/plugin.cpp
)

set(COMPONENT_HEADERS
    src/components/Button.h
    src/components/InputField.h
    src/components/StatusIndicator.h
    src/components/ProgressBar.h
    src/components/TabNavigation.h
    src/components/NotificationAlert.h
    src/components/Card.h
)

# QML files
set(QML_FILES
    src/components/Button.qml
    src/components/InputField.qml
    src/components/StatusIndicator.qml
    src/components/ProgressBar.qml
    src/components/TabNavigation.qml
    src/components/NotificationAlert.qml
    src/components/Card.qml
)

# Create Qt plugin for component library
add_library(miloscomponentsplugin SHARED
    ${COMPONENT_SOURCES}
    ${COMPONENT_HEADERS}
    ${QML_FILES}
    src/qml/MilosComponents.qmldir
    src/qml/plugin.cpp
)

target_link_libraries(miloscomponentsplugin
    Qt6::Core
    Qt6::Quick
    Qt6::QuickControls2
)

# Set plugin properties
set_target_properties(miloscomponentsplugin PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

# Install plugin
install(TARGETS miloscomponentsplugin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt6/qml/MilosComponents
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install QML files to components subdirectory
install(FILES ${QML_FILES}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt6/qml/MilosComponents
    RENAME ${QML_FILES}
)

# Install QML module files
install(FILES
    src/qml/MilosComponents.qmldir
    src/qml/MilosComponents.qmltypes
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt6/qml/MilosComponents
)

