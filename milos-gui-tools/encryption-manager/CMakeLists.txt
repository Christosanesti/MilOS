cmake_minimum_required(VERSION 3.27)
project(EncryptionManager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 components
# First try system Qt6, then check local installation
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

find_package(Qt6 REQUIRED COMPONENTS Core Widgets DBus)

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
set(SOURCES
    src/main.cpp
    src/ui/mainwindow.cpp
    src/ui/batchfilepicker.cpp
    src/services/encryptionmanagerservice.cpp
    src/services/singlefileencryption.cpp
    src/services/batchencryptionprocessor.cpp
)

set(HEADERS
    src/ui/mainwindow.h
    src/ui/batchfilepicker.h
    src/services/encryptionmanagerservice.h
    src/services/singlefileencryption.h
    src/services/batchencryptionprocessor.h
    src/dbus/encryptionmanager_interface.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link Qt6 libraries
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
    Qt6::DBus
)

# Install
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

