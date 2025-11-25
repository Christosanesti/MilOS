cmake_minimum_required(VERSION 3.27)
project(milos-theme-conflict-detection VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick DBus)

# Find yaml-cpp
find_package(yaml-cpp REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(SOURCES
    src/compatibilitydatabase.cpp
    src/conflictdetectionrules.cpp
    src/conflictnotifier.cpp
    src/fallbackstyling.cpp
    src/themeconflictdetector.cpp
    src/themevalidator.cpp
    src/notificationmanager.cpp
    src/qmltypes.cpp
)

set(HEADERS
    src/compatibilitydatabase.h
    src/conflictdetectionrules.h
    src/conflictnotifier.h
    src/fallbackstyling.h
    src/themeconflictdetector.h
    src/themevalidator.h
    src/notificationmanager.h
)

# Create library
add_library(milos-theme-conflict-detection STATIC
    ${SOURCES}
    ${HEADERS}
)

# Link libraries
target_link_libraries(milos-theme-conflict-detection
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    Qt6::DBus
    yaml-cpp::yaml-cpp
)

# Set properties
set_target_properties(milos-theme-conflict-detection
    PROPERTIES
    PUBLIC_HEADER "${HEADERS}"
)

# Install
install(TARGETS milos-theme-conflict-detection
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/milos/theme-conflict-detection
)

# Install QML files
install(FILES
    ui/CompatibilityIndicator.qml
    ui/CompatibilityReportDialog.qml
    ui/ThemeValidationDialog.qml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/milos/qml/ThemeConflict
)

