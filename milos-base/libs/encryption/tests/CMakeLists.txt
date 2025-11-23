cmake_minimum_required(VERSION 3.27)
project(EncryptionTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Test)
find_package(OpenSSL REQUIRED)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# File Encryption Tests
add_executable(test_file_encryption test_file_encryption.cpp)
target_link_libraries(test_file_encryption
    milos-file-encryption
    Qt6::Core
    Qt6::Test
    OpenSSL::SSL
    OpenSSL::Crypto
)
add_test(NAME FileEncryptionTest COMMAND test_file_encryption)

# Key Derivation Tests
add_executable(test_key_derivation test_key_derivation.cpp)
target_link_libraries(test_key_derivation
    milos-key-derivation
    Qt6::Core
    Qt6::Test
    OpenSSL::SSL
    OpenSSL::Crypto
)
add_test(NAME KeyDerivationTest COMMAND test_key_derivation)

# Operation Manager Tests
add_executable(test_operation_manager test_operation_manager.cpp)
target_link_libraries(test_operation_manager
    milos-encryption-ops
    Qt6::Core
    Qt6::Test
)
add_test(NAME OperationManagerTest COMMAND test_operation_manager)

# Disk Encryption Tests
add_executable(test_disk_encryption test_disk_encryption.cpp)
target_link_libraries(test_disk_encryption
    milos-disk-encryption
    Qt6::Core
    Qt6::Test
    OpenSSL::SSL
    OpenSSL::Crypto
)
if(CRYPTSETUP_FOUND)
    target_link_libraries(test_disk_encryption ${CRYPTSETUP_LIBRARIES})
    target_include_directories(test_disk_encryption PRIVATE ${CRYPTSETUP_INCLUDE_DIRS})
    target_compile_definitions(test_disk_encryption PRIVATE HAVE_CRYPTSETUP)
endif()
add_test(NAME DiskEncryptionTest COMMAND test_disk_encryption)

