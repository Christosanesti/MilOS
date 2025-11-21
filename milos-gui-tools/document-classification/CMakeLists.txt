cmake_minimum_required(VERSION 3.27)
project(MilosDocumentClassification VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Quick QuickControls2 DBus Sql)

# Find SQLite3
find_package(SQLite3 REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(APPLICATION_SOURCES
    src/main.cpp
    src/document_classification_system.cpp
    src/document_classifier.cpp
    src/classification_handler.cpp
    src/classification_storage.cpp
    src/dbus_interface.cpp
    src/audit_logger.cpp
)

# Header files
set(APPLICATION_HEADERS
    include/document_classification_system.h
    include/document_classifier.h
    include/classification_handler.h
    include/classification_storage.h
    include/dbus_interface.h
    include/audit_logger.h
)

# QML files
set(QML_FILES
    ui/main.qml
)

# Create executable
add_executable(milos-document-classification
    ${APPLICATION_SOURCES}
    ${APPLICATION_HEADERS}
)

# Enable MOC for Q_OBJECT classes
set_property(TARGET milos-document-classification PROPERTY AUTOMOC ON)

# Link libraries
target_link_libraries(milos-document-classification
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
    Qt6::Sql
    SQLite::SQLite3
)

# Add QML resources
qt_add_qml_module(milos-document-classification
    URI milos.documentclassification
    VERSION 1.0
    QML_FILES ${QML_FILES}
)

# Install targets
install(TARGETS milos-document-classification
    RUNTIME DESTINATION bin
)

