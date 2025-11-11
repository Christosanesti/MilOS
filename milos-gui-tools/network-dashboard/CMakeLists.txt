cmake_minimum_required(VERSION 3.27)
project(MilosNetworkDashboard VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Quick QuickControls2 DBus)

# Find Wireshark API (libwireshark)
pkg_check_modules(WIRESHARK wireshark)
if(NOT WIRESHARK_FOUND)
    # Try alternative names
    pkg_check_modules(WIRESHARK libwireshark)
    if(NOT WIRESHARK_FOUND)
        # Try to find via CMake
        find_path(WIRESHARK_INCLUDE_DIR wireshark/epan/packet.h
            PATHS
            /usr/include
            /usr/local/include
            /opt/wireshark/include
        )
        find_library(WIRESHARK_LIBRARY
            NAMES wireshark libwireshark
            PATHS
            /usr/lib
            /usr/local/lib
            /opt/wireshark/lib
        )
        if(WIRESHARK_INCLUDE_DIR AND WIRESHARK_LIBRARY)
            set(WIRESHARK_FOUND TRUE)
            set(WIRESHARK_INCLUDE_DIRS ${WIRESHARK_INCLUDE_DIR})
            set(WIRESHARK_LIBRARIES ${WIRESHARK_LIBRARY})
        endif()
    endif()
endif()

if(NOT WIRESHARK_FOUND)
    message(WARNING "Wireshark API not found. Packet capture will not be available.")
    message(STATUS "Install Wireshark development packages to enable packet capture.")
    add_definitions(-DHAVE_WIRESHARK=0)
else()
    add_definitions(-DHAVE_WIRESHARK=1)
    message(STATUS "Wireshark found: ${WIRESHARK_LIBRARIES}")
endif()

# Find libpcap (alternative to Wireshark for basic packet capture)
pkg_check_modules(LIBPCAP libpcap)
if(NOT LIBPCAP_FOUND)
    find_path(LIBPCAP_INCLUDE_DIR pcap/pcap.h
        PATHS
        /usr/include
        /usr/local/include
    )
    find_library(LIBPCAP_LIBRARY
        NAMES pcap
        PATHS
        /usr/lib
        /usr/local/lib
    )
    if(LIBPCAP_INCLUDE_DIR AND LIBPCAP_LIBRARY)
        set(LIBPCAP_FOUND TRUE)
        set(LIBPCAP_INCLUDE_DIRS ${LIBPCAP_INCLUDE_DIR})
        set(LIBPCAP_LIBRARIES ${LIBPCAP_LIBRARY})
    endif()
endif()

if(LIBPCAP_FOUND)
    message(STATUS "libpcap found: ${LIBPCAP_LIBRARIES}")
    add_definitions(-DHAVE_LIBPCAP=1)
else()
    add_definitions(-DHAVE_LIBPCAP=0)
endif()

# Find yaml-cpp for YAML configuration
find_package(yaml-cpp REQUIRED)

# Find CLI11 for CLI tool
find_package(CLI11 REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(WIRESHARK_FOUND)
    include_directories(${WIRESHARK_INCLUDE_DIRS})
endif()

if(LIBPCAP_FOUND)
    include_directories(${LIBPCAP_INCLUDE_DIRS})
endif()

# Source files
set(APPLICATION_SOURCES
    src/main.cpp
    src/network_dashboard.cpp
    src/wireshark_integration.cpp
    src/packet_capture.cpp
    src/packet_parser.cpp
    src/packet_statistics.cpp
    src/dbus_interface.cpp
    src/config_parser.cpp
    src/packet_sanitizer.cpp
    src/socket_interface.cpp
)

set(APPLICATION_HEADERS
    include/network_dashboard.h
    include/wireshark_integration.h
    include/packet_capture.h
    include/packet_parser.h
    include/packet_statistics.h
    include/dbus_interface.h
    include/config_parser.h
    include/packet_sanitizer.h
    include/socket_interface.h
)

# QML files
set(QML_FILES
    src/ui/main.qml
    src/ui/traffic_visualization.qml
    src/ui/network_statistics.qml
    src/ui/bandwidth_monitor.qml
    src/ui/connection_tracker.qml
    src/ui/network_alerts.qml
)

# Create executable
add_executable(milos-network-dashboard
    ${APPLICATION_SOURCES}
    ${APPLICATION_HEADERS}
    ${QML_FILES}
)

# Link libraries
target_link_libraries(milos-network-dashboard
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
    yaml-cpp::yaml-cpp
    CLI11::CLI11
)

if(WIRESHARK_FOUND)
    target_link_libraries(milos-network-dashboard ${WIRESHARK_LIBRARIES})
    target_include_directories(milos-network-dashboard PRIVATE ${WIRESHARK_INCLUDE_DIRS})
endif()

if(LIBPCAP_FOUND)
    target_link_libraries(milos-network-dashboard ${LIBPCAP_LIBRARIES})
    target_include_directories(milos-network-dashboard PRIVATE ${LIBPCAP_INCLUDE_DIRS})
endif()

# Enable Qt MOC and QRC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set_property(TARGET milos-network-dashboard PROPERTY AUTOMOC ON)

# Create QML resource file
qt6_add_resources(milos-network-dashboard "qml"
    PREFIX "/"
    FILES ${QML_FILES}
)

# Install executable
install(TARGETS milos-network-dashboard
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install configuration template
install(FILES data/config.yaml.template
    DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/milos/network-dashboard
    RENAME config.yaml
)

# Create CLI tool executable
add_executable(milos-network-monitor
    src/cli_tool.cpp
)

target_link_libraries(milos-network-monitor
    Qt6::Core
    Qt6::DBus
    CLI11::CLI11
)

set_property(TARGET milos-network-monitor PROPERTY AUTOMOC ON)

install(TARGETS milos-network-monitor
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Testing
enable_testing()
add_subdirectory(tests)

