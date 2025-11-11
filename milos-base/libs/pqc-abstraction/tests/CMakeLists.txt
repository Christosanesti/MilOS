cmake_minimum_required(VERSION 3.27)
project(PQCAbstractionTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Test)
find_package(yaml-cpp REQUIRED)

# Include the library's source directory to access headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Algorithm Factory Tests
add_executable(test_algorithm_factory test_algorithm_factory.cpp)
target_link_libraries(test_algorithm_factory
    Qt6::Core
    Qt6::Test
    yaml-cpp::yaml-cpp
)
add_test(NAME AlgorithmFactoryTest COMMAND test_algorithm_factory)

# Algorithm Registry Tests
add_executable(test_algorithm_registry test_algorithm_registry.cpp)
target_link_libraries(test_algorithm_registry
    Qt6::Core
    Qt6::Test
    yaml-cpp::yaml-cpp
)
add_test(NAME AlgorithmRegistryTest COMMAND test_algorithm_registry)

# Algorithm Versioning Tests
add_executable(test_algorithm_versioning test_algorithm_versioning.cpp)
target_link_libraries(test_algorithm_versioning
    Qt6::Core
    Qt6::Test
)
add_test(NAME AlgorithmVersioningTest COMMAND test_algorithm_versioning)

# Hybrid Encryption Tests
add_executable(test_hybrid_encryption test_hybrid_encryption.cpp)
target_link_libraries(test_hybrid_encryption
    Qt6::Core
    Qt6::Test
)
add_test(NAME HybridEncryptionTest COMMAND test_hybrid_encryption)

# Config Parser Tests
add_executable(test_config_parser test_config_parser.cpp)
target_link_libraries(test_config_parser
    Qt6::Core
    Qt6::Test
    yaml-cpp::yaml-cpp
)
add_test(NAME ConfigParserTest COMMAND test_config_parser)

# Performance Optimizer Tests
add_executable(test_performance_optimizer test_performance_optimizer.cpp)
target_link_libraries(test_performance_optimizer
    Qt6::Core
    Qt6::Test
)
add_test(NAME PerformanceOptimizerTest COMMAND test_performance_optimizer)

# Algorithm Implementation Tests (if liboqs available)
if(LIBOQS_FOUND)
    # Kyber Algorithm Tests
    add_executable(test_kyber_algorithm test_kyber_algorithm.cpp)
    target_link_libraries(test_kyber_algorithm
        milos-pqc
        Qt6::Core
        Qt6::Test
        ${LIBOQS_LIBRARIES}
    )
    target_include_directories(test_kyber_algorithm PRIVATE ${LIBOQS_INCLUDE_DIRS})
    target_compile_definitions(test_kyber_algorithm PRIVATE HAVE_LIBOQS)
    add_test(NAME KyberAlgorithmTest COMMAND test_kyber_algorithm)
endif()

