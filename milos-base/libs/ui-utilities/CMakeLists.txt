cmake_minimum_required(VERSION 3.27)
project(milos-ui-utilities)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core DBus Network OpenGL)

# Find required system libraries
find_library(DL_LIBRARY dl)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/error_handler.cpp
    src/fallback_manager.cpp
    src/crash_handler.cpp
    src/bug_reporter.cpp
)

set(HEADERS
    include/milos/ui/error_handler.h
    include/milos/ui/fallback_manager.h
    include/milos/ui/crash_handler.h
    include/milos/ui/bug_reporter.h
)

# Create library
add_library(milos-ui-utilities STATIC ${SOURCES} ${HEADERS})

target_link_libraries(milos-ui-utilities
    Qt6::Core
    Qt6::DBus
    Qt6::Network
    Qt6::OpenGL
    milos-logging
    ${DL_LIBRARY}
)

# Install
install(TARGETS milos-ui-utilities
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${HEADERS}
    DESTINATION include/milos/ui
)



set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core DBus Network OpenGL)

# Find required system libraries
find_library(DL_LIBRARY dl)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/error_handler.cpp
    src/fallback_manager.cpp
    src/crash_handler.cpp
    src/bug_reporter.cpp
)

set(HEADERS
    include/milos/ui/error_handler.h
    include/milos/ui/fallback_manager.h
    include/milos/ui/crash_handler.h
    include/milos/ui/bug_reporter.h
)

# Create library
add_library(milos-ui-utilities STATIC ${SOURCES} ${HEADERS})

target_link_libraries(milos-ui-utilities
    Qt6::Core
    Qt6::DBus
    Qt6::Network
    Qt6::OpenGL
    milos-logging
    ${DL_LIBRARY}
)

# Install
install(TARGETS milos-ui-utilities
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${HEADERS}
    DESTINATION include/milos/ui
)

