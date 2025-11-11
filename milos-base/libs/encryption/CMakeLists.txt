cmake_minimum_required(VERSION 3.27)
project(MilosEncryption VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

# Find PQC abstraction layer (from Story 17.1)
find_package(milos-pqc QUIET)
if(NOT milos-pqc_FOUND)
    # Try to find it in the build tree
    find_path(MILOS_PQC_INCLUDE_DIR milos/pqc/algorithm_interface.h
        PATHS
        ${CMAKE_CURRENT_SOURCE_DIR}/../pqc-abstraction/include
        ${CMAKE_SOURCE_DIR}/milos-base/libs/pqc-abstraction/include
    )
    find_library(MILOS_PQC_LIBRARY
        NAMES milos-pqc
        PATHS
        ${CMAKE_CURRENT_SOURCE_DIR}/../pqc-abstraction
        ${CMAKE_SOURCE_DIR}/milos-base/libs/pqc-abstraction
    )
    if(MILOS_PQC_INCLUDE_DIR AND MILOS_PQC_LIBRARY)
        set(milos-pqc_FOUND TRUE)
        set(MILOS_PQC_INCLUDE_DIRS ${MILOS_PQC_INCLUDE_DIR})
        set(MILOS_PQC_LIBRARIES ${MILOS_PQC_LIBRARY})
    endif()
endif()

if(NOT milos-pqc_FOUND)
    message(FATAL_ERROR "PQC abstraction layer (milos-pqc) not found. Please build Story 17.1 first.")
endif()

# Find OpenSSL for classical encryption fallback
find_package(OpenSSL REQUIRED)

# Find LUKS2 (cryptsetup) for disk encryption
pkg_check_modules(CRYPTSETUP cryptsetup)
if(NOT CRYPTSETUP_FOUND)
    message(WARNING "cryptsetup not found. Disk encryption will not be available.")
    add_definitions(-DHAVE_CRYPTSETUP=0)
else()
    add_definitions(-DHAVE_CRYPTSETUP=1)
    message(STATUS "cryptsetup found: ${CRYPTSETUP_LIBRARIES}")
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${MILOS_PQC_INCLUDE_DIRS}
)

# Source files for shared libraries
set(FILE_ENCRYPTION_SOURCES
    src/file_encryption.cpp
)

set(DISK_ENCRYPTION_SOURCES
    src/disk_encryption.cpp
)

set(KEY_DERIVATION_SOURCES
    src/key_derivation.cpp
)

set(OPERATION_MANAGER_SOURCES
    src/operation_manager.cpp
)

set(ENCRYPTION_HEADERS
    include/milos/encryption/file_encryption.h
    include/milos/encryption/disk_encryption.h
    include/milos/encryption/key_derivation.h
    include/milos/encryption/operation_manager.h
)

# Create file encryption library
add_library(milos-file-encryption SHARED
    ${FILE_ENCRYPTION_SOURCES}
)

target_link_libraries(milos-file-encryption
    ${MILOS_PQC_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
)

target_include_directories(milos-file-encryption PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${MILOS_PQC_INCLUDE_DIRS}
)

# Create disk encryption library
add_library(milos-disk-encryption SHARED
    ${DISK_ENCRYPTION_SOURCES}
)

target_link_libraries(milos-disk-encryption
    ${MILOS_PQC_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
)

if(CRYPTSETUP_FOUND)
    target_link_libraries(milos-disk-encryption
        ${CRYPTSETUP_LIBRARIES}
    )
    target_include_directories(milos-disk-encryption PRIVATE ${CRYPTSETUP_INCLUDE_DIRS})
endif()

target_include_directories(milos-disk-encryption PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${MILOS_PQC_INCLUDE_DIRS}
)

# Create key derivation library
add_library(milos-key-derivation SHARED
    ${KEY_DERIVATION_SOURCES}
)

target_link_libraries(milos-key-derivation
    ${MILOS_PQC_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
)

target_include_directories(milos-key-derivation PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${MILOS_PQC_INCLUDE_DIRS}
)

# Create operation manager library
add_library(milos-encryption-ops SHARED
    ${OPERATION_MANAGER_SOURCES}
)

target_link_libraries(milos-encryption-ops
    ${MILOS_PQC_LIBRARIES}
)

target_include_directories(milos-encryption-ops PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${MILOS_PQC_INCLUDE_DIRS}
)

# Set library properties
set_target_properties(
    milos-file-encryption
    milos-disk-encryption
    milos-key-derivation
    milos-encryption-ops
    PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${ENCRYPTION_HEADERS}"
)

# Install libraries
install(TARGETS
    milos-file-encryption
    milos-disk-encryption
    milos-key-derivation
    milos-encryption-ops
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/milos/encryption
)

# Testing
enable_testing()
add_subdirectory(tests)

