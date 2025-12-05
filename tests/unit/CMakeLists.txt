cmake_minimum_required(VERSION 3.16)

# Enable testing
enable_testing()

# Find Qt6 Test
find_package(Qt6 REQUIRED COMPONENTS Core Test)

# Logging library is added at the top level

# Include directories - CMAKE_SOURCE_DIR is the tests directory, need to go up one level
set(MILOS_ROOT ${CMAKE_SOURCE_DIR}/..)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${MILOS_ROOT}/milos-base/services/data-guard/src
    ${MILOS_ROOT}/milos-base/services/stig-compliance-checker/include
    ${MILOS_ROOT}/milos-base/services/air-gap-deployment-manager/include
    ${MILOS_ROOT}/milos-base/libs/logging/include
)

# Find yaml-cpp for policy manager tests
find_package(yaml-cpp QUIET)
if(NOT yaml-cpp_FOUND)
    if(PkgConfig_FOUND)
        pkg_check_modules(YAML_CPP yaml-cpp)
        if(NOT YAML_CPP_FOUND)
            message(WARNING "yaml-cpp not found. Policy manager tests may not compile.")
        endif()
    else()
        message(WARNING "yaml-cpp not found and pkg-config unavailable. Policy manager tests may not compile.")
    endif()
endif()

# Find libpcap for network enforcement tests
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(LIBPCAP libpcap)
    if(NOT LIBPCAP_FOUND)
        message(WARNING "libpcap not found. Network enforcement tests may not compile.")
    endif()
else()
    message(WARNING "pkg-config not found. Some dependencies may not be found.")
endif()

# D-Bus Test Harness Tests
add_executable(test_dbus_harness
    test_dbus_harness.cpp
)
target_link_libraries(test_dbus_harness
    Qt6::Core
    Qt6::DBus
    Qt6::Test
    dbus-test-harness
    milos-logging
)
set_property(TARGET test_dbus_harness PROPERTY AUTOMOC ON)
add_test(NAME DBusHarnessTest COMMAND test_dbus_harness)

# Audit Logger Tests
add_executable(test_audit_logger
    test_audit_logger.cpp
    ../../milos-base/services/data-guard/src/audit_logger.cpp
)
target_link_libraries(test_audit_logger
    Qt6::Core
    Qt6::DBus
    Qt6::Test
    dbus-test-harness
    milos-logging
)
target_include_directories(test_audit_logger PRIVATE
    ${MILOS_ROOT}/milos-base/libs/logging/include
    ${MILOS_ROOT}/milos-base/services/data-guard/src
)
set_property(TARGET test_audit_logger PROPERTY AUTOMOC ON)
add_test(NAME AuditLoggerTest COMMAND test_audit_logger)

# Policy Manager Tests
add_executable(test_policy_manager
    test_policy_manager.cpp
    ../../milos-base/services/data-guard/src/policy_manager.cpp
    ../../milos-base/services/data-guard/src/config_parser.cpp
)
target_link_libraries(test_policy_manager
    Qt6::Core
    Qt6::Test
    milos-logging
)
target_include_directories(test_policy_manager PRIVATE
    ${MILOS_ROOT}/milos-base/libs/logging/include
    ${MILOS_ROOT}/milos-base/services/data-guard/src
)
if(yaml-cpp_FOUND)
    target_link_libraries(test_policy_manager yaml-cpp::yaml-cpp)
elseif(YAML_CPP_FOUND)
    target_link_libraries(test_policy_manager ${YAML_CPP_LIBRARIES})
    target_include_directories(test_policy_manager PRIVATE ${YAML_CPP_INCLUDE_DIRS})
endif()
set_property(TARGET test_policy_manager PROPERTY AUTOMOC ON)
add_test(NAME PolicyManagerTest COMMAND test_policy_manager)

# Network Enforcement Tests
add_executable(test_network_enforcement
    test_network_enforcement.cpp
    ../../milos-base/services/data-guard/src/network_enforcement.cpp
    ../../milos-base/services/data-guard/src/policy_manager.cpp
    ../../milos-base/services/data-guard/src/config_parser.cpp
    ../../milos-base/services/data-guard/src/audit_logger.cpp
)
target_link_libraries(test_network_enforcement
    Qt6::Core
    Qt6::DBus
    Qt6::Test
    dbus-test-harness
    milos-logging
    pthread
)
target_include_directories(test_network_enforcement PRIVATE
    ${MILOS_ROOT}/milos-base/libs/logging/include
    ${MILOS_ROOT}/milos-base/services/data-guard/src
)
if(yaml-cpp_FOUND)
    target_link_libraries(test_network_enforcement yaml-cpp::yaml-cpp)
elseif(YAML_CPP_FOUND)
    target_link_libraries(test_network_enforcement ${YAML_CPP_LIBRARIES})
    target_include_directories(test_network_enforcement PRIVATE ${YAML_CPP_INCLUDE_DIRS})
endif()
if(LIBPCAP_FOUND)
    target_link_libraries(test_network_enforcement ${LIBPCAP_LIBRARIES})
    target_include_directories(test_network_enforcement PRIVATE ${LIBPCAP_INCLUDE_DIRS})
endif()
set_property(TARGET test_network_enforcement PROPERTY AUTOMOC ON)
add_test(NAME NetworkEnforcementTest COMMAND test_network_enforcement)

# STIG Compliance Checker Tests
add_executable(test_stig_checker
    test_stig_checker.cpp
    ../../milos-base/services/stig-compliance-checker/src/stig_compliance_checker.cpp
    ../../milos-base/services/stig-compliance-checker/include/stig_compliance_checker.h
)
target_link_libraries(test_stig_checker
    Qt6::Core
    Qt6::Test
    milos-logging
)
target_include_directories(test_stig_checker PRIVATE
    ${MILOS_ROOT}/milos-base/libs/logging/include
    ${MILOS_ROOT}/milos-base/services/stig-compliance-checker/include
)
set_property(TARGET test_stig_checker PROPERTY AUTOMOC ON)
add_test(NAME STIGCheckerTest COMMAND test_stig_checker)

# Air-Gap Deployment Manager Tests
add_executable(test_air_gap_manager
    test_air_gap_manager.cpp
    ../../milos-base/services/air-gap-deployment-manager/src/air_gap_deployment_manager.cpp
    ../../milos-base/services/air-gap-deployment-manager/include/air_gap_deployment_manager.h
)
target_link_libraries(test_air_gap_manager
    Qt6::Core
    Qt6::Test
    milos-logging
)
target_include_directories(test_air_gap_manager PRIVATE
    ${MILOS_ROOT}/milos-base/libs/logging/include
    ${MILOS_ROOT}/milos-base/services/air-gap-deployment-manager/include
)
set_property(TARGET test_air_gap_manager PROPERTY AUTOMOC ON)
add_test(NAME AirGapManagerTest COMMAND test_air_gap_manager)

# Data Guard Service Tests
if(EXISTS "${CMAKE_SOURCE_DIR}/milos-base/services/data-guard/tests")
    add_subdirectory(${CMAKE_SOURCE_DIR}/milos-base/services/data-guard/tests ${CMAKE_BINARY_DIR}/data-guard-tests)
endif()

# STIG Compliance Checker Tests
if(EXISTS "${CMAKE_SOURCE_DIR}/milos-base/services/stig-compliance-checker/tests")
    add_subdirectory(${CMAKE_SOURCE_DIR}/milos-base/services/stig-compliance-checker/tests ${CMAKE_BINARY_DIR}/stig-checker-tests)
endif()

# Air-Gap Deployment Manager Tests
if(EXISTS "${CMAKE_SOURCE_DIR}/milos-base/services/air-gap-deployment-manager/tests")
    add_subdirectory(${CMAKE_SOURCE_DIR}/milos-base/services/air-gap-deployment-manager/tests ${CMAKE_BINARY_DIR}/air-gap-tests)
endif()

# Code Coverage Configuration
option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)

if(ENABLE_COVERAGE)
    # Check for gcov
    find_program(GCOV_PATH gcov)
    if(NOT GCOV_PATH)
        message(WARNING "gcov not found. Code coverage will not be available.")
    else()
        # Add coverage flags
        set(CMAKE_CXX_FLAGS_COVERAGE "-g -O0 --coverage -fprofile-arcs -ftest-coverage")
        set(CMAKE_C_FLAGS_COVERAGE "-g -O0 --coverage -fprofile-arcs -ftest-coverage")
        set(CMAKE_EXE_LINKER_FLAGS_COVERAGE "--coverage")
        
        # Check for lcov
        find_program(LCOV_PATH lcov)
        find_program(GENHTML_PATH genhtml)
        
        if(LCOV_PATH AND GENHTML_PATH)
            # Create coverage target
            add_custom_target(coverage
                COMMAND ${LCOV_PATH} --directory ${CMAKE_BINARY_DIR} --capture --output-file ${CMAKE_BINARY_DIR}/coverage.info
                COMMAND ${LCOV_PATH} --remove ${CMAKE_BINARY_DIR}/coverage.info '/usr/*' '*/tests/*' --output-file ${CMAKE_BINARY_DIR}/coverage.info
                COMMAND ${GENHTML_PATH} ${CMAKE_BINARY_DIR}/coverage.info --output-directory ${CMAKE_BINARY_DIR}/coverage
                COMMAND ${CMAKE_COMMAND} -E echo "Coverage report generated in ${CMAKE_BINARY_DIR}/coverage/index.html"
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating code coverage report"
            )
            
            message(STATUS "Code coverage enabled. Run 'make coverage' to generate report.")
        else()
            message(WARNING "lcov or genhtml not found. HTML coverage reports will not be available.")
        endif()
    endif()
endif()

