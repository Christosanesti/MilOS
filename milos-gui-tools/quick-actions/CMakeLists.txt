cmake_minimum_required(VERSION 3.27)
project(QuickActions VERSION 1.0.0 LANGUAGES CXX)

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

find_package(Qt6 REQUIRED COMPONENTS Core Quick QuickControls2 DBus Gui)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(QUICK_ACTIONS_SOURCES
    src/quickactionsservice.cpp
    src/main.cpp
)

set(QUICK_ACTIONS_HEADERS
    src/quickactionsservice.h
)

# QML files
set(QUICK_ACTIONS_QML
    ui/main.qml
    ui/FloatingActionButtons.qml
    ui/FloatingActionButton.qml
    ui/ConfigDialog.qml
)

# Create executable
add_executable(quick-actions
    ${QUICK_ACTIONS_SOURCES}
    ${QUICK_ACTIONS_HEADERS}
    ${QUICK_ACTIONS_QML}
)

target_link_libraries(quick-actions
    Qt6::Core
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
    Qt6::Gui
)

# Install
install(TARGETS quick-actions
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

