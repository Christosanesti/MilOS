cmake_minimum_required(VERSION 3.27)
project(AuditServiceTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Test)
find_package(SQLite3 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(yaml-cpp REQUIRED)

# Include the service's source directory to access headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Log Storage Tests
add_executable(test_log_storage test_log_storage.cpp)
target_link_libraries(test_log_storage
    Qt6::Core
    Qt6::Test
    SQLite::SQLite3
    OpenSSL::SSL
    OpenSSL::Crypto
    yaml-cpp::yaml-cpp
)
add_test(NAME LogStorageTest COMMAND test_log_storage)

# Hash Chain Tests
add_executable(test_hash_chain test_hash_chain.cpp)
target_link_libraries(test_hash_chain
    Qt6::Core
    Qt6::Test
    SQLite::SQLite3
    OpenSSL::SSL
    OpenSSL::Crypto
    yaml-cpp::yaml-cpp
)
add_test(NAME HashChainTest COMMAND test_hash_chain)

# Event Collector Tests
add_executable(test_event_collector test_event_collector.cpp)
target_link_libraries(test_event_collector
    Qt6::Core
    Qt6::Test
    Qt6::DBus
    SQLite::SQLite3
    OpenSSL::SSL
    OpenSSL::Crypto
    yaml-cpp::yaml-cpp
)
add_test(NAME EventCollectorTest COMMAND test_event_collector)

# Query Engine Tests
add_executable(test_query_engine test_query_engine.cpp)
target_link_libraries(test_query_engine
    Qt6::Core
    Qt6::Test
    SQLite::SQLite3
    OpenSSL::SSL
    OpenSSL::Crypto
    yaml-cpp::yaml-cpp
)
add_test(NAME QueryEngineTest COMMAND test_query_engine)

# Config Parser Tests
add_executable(test_config_parser test_config_parser.cpp)
target_link_libraries(test_config_parser
    Qt6::Core
    Qt6::Test
    yaml-cpp::yaml-cpp
)
add_test(NAME ConfigParserTest COMMAND test_config_parser)

