cmake_minimum_required(VERSION 3.27)
project(milos-logging)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core DBus)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/logger.cpp
)

set(HEADERS
    include/milos/logging/logger.h
)

# Create library
add_library(milos-logging STATIC ${SOURCES} ${HEADERS})

target_link_libraries(milos-logging
    Qt6::Core
    Qt6::DBus
)

# Install
install(TARGETS milos-logging
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${HEADERS}
    DESTINATION include/milos/logging
)



project(milos-logging)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core DBus)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/logger.cpp
)

set(HEADERS
    include/milos/logging/logger.h
)

# Create library
add_library(milos-logging STATIC ${SOURCES} ${HEADERS})

target_link_libraries(milos-logging
    Qt6::Core
    Qt6::DBus
)

# Install
install(TARGETS milos-logging
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${HEADERS}
    DESTINATION include/milos/logging
)


