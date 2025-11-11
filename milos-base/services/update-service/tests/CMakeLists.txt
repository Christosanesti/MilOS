cmake_minimum_required(VERSION 3.27)
project(UpdateServiceTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Test)
find_package(SQLite3 REQUIRED)
find_package(yaml-cpp REQUIRED)

# Include the service's source directory to access headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Config Parser Tests
add_executable(test_config_parser test_config_parser.cpp)
target_link_libraries(test_config_parser
    Qt6::Core
    Qt6::Test
    yaml-cpp::yaml-cpp
)
add_test(NAME ConfigParserTest COMMAND test_config_parser)

# Package Manager Tests (structure created, requires package manager for full testing)
# Note: Full testing requires actual package manager (pacman/APT) environment

# Signature Verifier Tests (structure created, requires GPG environment)
# Note: Full testing requires GPG keyring and signed packages

# Update Applier Tests (structure created, requires package manager for full testing)
# Note: Full testing requires actual package manager environment

# Rollback Manager Tests (structure created, requires package manager for full testing)
# Note: Full testing requires actual package manager environment

