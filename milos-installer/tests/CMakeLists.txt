cmake_minimum_required(VERSION 3.27)

# Find Qt6 Test
find_package(Qt6 REQUIRED COMPONENTS Test Quick QuickControls2)

# Test executable
add_executable(milos-installer-tests
    test_main.cpp
    test_welcomescreen.cpp
    test_partitioningscreen.cpp
    test_encryptionsetupscreen.cpp
    test_useraccountscreen.cpp
    test_networkconfigurationscreen.cpp
    test_hardwaredetectionscreen.cpp
    test_packageinstallationscreen.cpp
    test_installationcompletionscreen.cpp
    test_wizardnavigation.cpp
    test_installerstate.cpp
    test_accessibility.cpp
    test_integration.cpp
)

# Link libraries
target_link_libraries(milos-installer-tests
    Qt6::Core
    Qt6::Test
    Qt6::Quick
    Qt6::QuickControls2
)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)
set_property(TARGET milos-installer-tests PROPERTY AUTOMOC ON)

# Add test to CTest
add_test(NAME milos-installer-tests COMMAND milos-installer-tests)

