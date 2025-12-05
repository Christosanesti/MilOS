cmake_minimum_required(VERSION 3.27)
project(blackarch-integration)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core DBus Network)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(SOURCES
    src/blackarch_repository.cpp
    src/blackarch_offline_installer.cpp
    src/blackarch_tool_manager.cpp
    src/blackarch_data_scraper.cpp
)

# Header files
set(HEADERS
    include/blackarch_repository.h
    include/blackarch_offline_installer.h
    include/blackarch_tool_manager.h
    include/blackarch_data_scraper.h
)

# Create library
add_library(blackarch-integration STATIC
    ${SOURCES}
    ${HEADERS}
)

# Link logging library
add_subdirectory(../../libs/logging ${CMAKE_BINARY_DIR}/libs/logging)

# Link libraries
target_link_libraries(blackarch-integration
    Qt6::Core
    Qt6::DBus
    Qt6::Network
    milos::logging
)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)
set_property(TARGET blackarch-integration PROPERTY AUTOMOC ON)

# Install
install(TARGETS blackarch-integration
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${HEADERS}
    DESTINATION include/blackarch-integration
)

