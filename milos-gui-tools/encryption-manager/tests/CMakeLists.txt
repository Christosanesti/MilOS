cmake_minimum_required(VERSION 3.27)
project(EncryptionManagerTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Test configuration
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Test DBus)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/services
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/dbus
)

# BatchFilePicker test executable
add_executable(batchfilepicker_test
    batchfilepicker_test.cpp
    ../src/ui/batchfilepicker.cpp
)

# Link Qt6 libraries for batchfilepicker_test
target_link_libraries(batchfilepicker_test
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

# Add batchfilepicker test
add_test(NAME BatchFilePickerTest COMMAND batchfilepicker_test)

# D-Bus EncryptionManager test executable
add_executable(dbus_encryptionmanager_test
    dbus_encryptionmanager_test.cpp
    ../src/services/encryptionmanagerservice.cpp
    ../src/services/batchencryptionprocessor.cpp
    ../src/services/singlefileencryption.cpp
)

# Link Qt6 libraries for dbus_encryptionmanager_test
target_link_libraries(dbus_encryptionmanager_test
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
    Qt6::DBus
)

# Add D-Bus test
add_test(NAME DBusEncryptionManagerTest COMMAND dbus_encryptionmanager_test)

# Batch Encryption Processor test executable
add_executable(batchencryptionprocessor_test
    batchencryptionprocessor_test.cpp
    ../src/services/batchencryptionprocessor.cpp
    ../src/services/singlefileencryption.cpp
)

# Link Qt6 libraries for batchencryptionprocessor_test
target_link_libraries(batchencryptionprocessor_test
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
    Qt6::DBus
)

# Add batch processor test
add_test(NAME BatchEncryptionProcessorTest COMMAND batchencryptionprocessor_test)

