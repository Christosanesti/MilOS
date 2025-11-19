cmake_minimum_required(VERSION 3.27)
project(MilosPersonnelScheduler VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Quick QuickControls2 DBus)

# Find yaml-cpp for YAML configuration
find_package(yaml-cpp REQUIRED)

# Find CLI11 for CLI tool
find_package(CLI11 REQUIRED)

# Find SQLite3 for device configuration and status
find_package(SQLite3 REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Sql)

# Optional: Find libfprint for fingerprint scanner support
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBFPRINT libfprint)

# Optional: Find OpenCV for face recognition support
find_package(OpenCV QUIET)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(APPLICATION_SOURCES
    src/main.cpp
    src/personnel_scheduler.cpp
    src/biometric_abstraction.cpp
    src/face_recognition_device.cpp
    src/fingerprint_device.cpp
    src/device_manager.cpp
    src/device_health.cpp
    src/dbus_interface.cpp
    src/cli_tool.cpp
    src/config_parser.cpp
    src/audit_logger.cpp
    src/attendance_tracker.cpp
    src/biometric_capture.cpp
    src/entry_exit_logger.cpp
    src/attendance_storage.cpp
    src/attendance_validator.cpp
    src/shift_scheduler.cpp
    src/conflict_detector.cpp
    src/leave_manager.cpp
    src/shift_swap.cpp
    src/coverage_manager.cpp
    src/access_control.cpp
    src/access_restrictions.cpp
    src/role_manager.cpp
    src/report_generator.cpp
    src/analytics_engine.cpp
    src/export_manager.cpp
    src/compliance_reporter.cpp
    src/personnel_integration.cpp
)

# Header files
set(APPLICATION_HEADERS
    include/personnel_scheduler.h
    include/biometric_abstraction.h
    include/face_recognition_device.h
    include/fingerprint_device.h
    include/device_manager.h
    include/device_health.h
    include/dbus_interface.h
    include/config_parser.h
    include/audit_logger.h
    include/attendance_tracker.h
    include/biometric_capture.h
    include/entry_exit_logger.h
    include/attendance_storage.h
    include/attendance_validator.h
    include/shift_scheduler.h
    include/conflict_detector.h
    include/leave_manager.h
    include/shift_swap.h
    include/coverage_manager.h
    include/access_control.h
    include/access_restrictions.h
    include/role_manager.h
    include/report_generator.h
    include/analytics_engine.h
    include/export_manager.h
    include/compliance_reporter.h
    include/personnel_integration.h
)

# QML files
set(QML_FILES
    ui/main.qml
    ui/DeviceStatusDashboard.qml
    ui/DeviceConfiguration.qml
    ui/attendance_tracking.qml
    ui/scheduling_calendar.qml
    ui/shift_management.qml
    ui/access_control.qml
    ui/reporting_dashboard.qml
    ui/analytics_dashboard.qml
)

# Create executable
add_executable(milos-personnel-scheduler
    ${APPLICATION_SOURCES}
    ${APPLICATION_HEADERS}
    ${QML_FILES}
)

# Link libraries
target_link_libraries(milos-personnel-scheduler
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
    Qt6::Sql
    yaml-cpp
    CLI11::CLI11
    SQLite::SQLite3
    milos-pqc
)

# Link optional libraries if available
if(LIBFPRINT_FOUND)
    target_link_libraries(milos-personnel-scheduler ${LIBFPRINT_LIBRARIES})
    target_include_directories(milos-personnel-scheduler PRIVATE ${LIBFPRINT_INCLUDE_DIRS})
    target_compile_definitions(milos-personnel-scheduler PRIVATE HAVE_LIBFPRINT)
endif()

if(OpenCV_FOUND)
    target_link_libraries(milos-personnel-scheduler ${OpenCV_LIBS})
    target_include_directories(milos-personnel-scheduler PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_compile_definitions(milos-personnel-scheduler PRIVATE HAVE_OPENCV)
endif()

# Set QML import path
set_target_properties(milos-personnel-scheduler PROPERTIES
    Qt6_QML_IMPORT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ui"
)

# Install executable
install(TARGETS milos-personnel-scheduler
    RUNTIME DESTINATION bin
)

# Install QML files
install(FILES ${QML_FILES}
    DESTINATION share/milos/personnel-scheduler/ui
)

# Install data files
install(FILES data/config.yaml.template
    DESTINATION share/milos/personnel-scheduler/data
)

# CLI tool executable
add_executable(milos-personnel
    src/cli_tool.cpp
    src/dbus_interface.cpp
    src/audit_logger.cpp
    include/dbus_interface.h
    include/audit_logger.h
)

target_link_libraries(milos-personnel
    Qt6::Core
    Qt6::DBus
    CLI11::CLI11
)

install(TARGETS milos-personnel
    RUNTIME DESTINATION bin
)


