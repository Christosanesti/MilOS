cmake_minimum_required(VERSION 3.27)

find_package(Qt6 REQUIRED COMPONENTS Core Test)

add_executable(performance-mode-test
    test_main.cpp
)

target_link_libraries(performance-mode-test
    milos-performance-mode
    Qt6::Core
    Qt6::Test
)

add_test(NAME PerformanceModeTest COMMAND performance-mode-test)

