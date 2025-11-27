cmake_minimum_required(VERSION 3.27)
project(blackarch-launcher)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick QuickControls2 DBus)

# Find KDE Frameworks
find_package(KF6 REQUIRED COMPONENTS CoreAddons)

# Find CLI11 for CLI tool
find_package(CLI11 REQUIRED)

# Find BlackArch integration library
find_package(blackarch-integration REQUIRED)

# Link logging and UI utilities libraries
add_subdirectory(../../base/libs/logging ${CMAKE_BINARY_DIR}/libs/logging)
add_subdirectory(../../base/libs/ui-utilities ${CMAKE_BINARY_DIR}/libs/ui-utilities)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(APPLICATION_SOURCES
    src/main.cpp
    src/launcher.cpp
    src/tool_discovery.cpp
    src/tool_manager.cpp
    src/dbus_interface.cpp
    src/cli_tool.cpp
)

set(APPLICATION_HEADERS
    include/launcher.h
    include/tool_discovery.h
    include/tool_manager.h
    include/dbus_interface.h
)

# QML files
set(QML_FILES
    ui/main.qml
    ui/ToolList.qml
    ui/ToolCard.qml
    ui/CategoryFilter.qml
    ui/SearchBar.qml
    ui/FavoritesPanel.qml
)

# Create executable
add_executable(milos-blackarch-launcher
    ${APPLICATION_SOURCES}
    ${APPLICATION_HEADERS}
    ${QML_FILES}
)

# Link libraries
target_link_libraries(milos-blackarch-launcher
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
    KF6::CoreAddons
    blackarch-integration
    CLI11::CLI11
    milos::logging
    milos-ui-utilities
)

# Enable Qt MOC and QRC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set_property(TARGET milos-blackarch-launcher PROPERTY AUTOMOC ON)

# Install
install(TARGETS milos-blackarch-launcher
    RUNTIME DESTINATION bin
)

