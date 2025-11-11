# MilOS Security Services Integration Tests

This directory contains integration tests for verifying that all three security services (Data Transmission Guard, Audit Service, Update Service) work together correctly.

## Test Structure

- **test_service_integration.cpp** - Tests service-to-service communication
- **test_dbus_interfaces.cpp** - Tests D-Bus interface functionality
- **test_cli_tools.cpp** - Tests CLI tool functionality
- **test_graceful_degradation.cpp** - Tests graceful degradation scenarios
- **verify_service_isolation.sh** - Script to verify service isolation

## Building Tests

Tests are built as part of the main CMake build:

```bash
cd milos-base/services/integration-tests
mkdir build
cd build
cmake ..
make
```

## Running Tests

### Run all tests using CTest:

```bash
cd build
ctest
```

### Run individual test executables:

```bash
cd build
./test_service_integration
./test_dbus_interfaces
./test_cli_tools
./test_graceful_degradation
```

### Verify Service Isolation:

```bash
./verify_service_isolation.sh
```

## Test Requirements

### Service Integration Tests
- All three services must be running
- D-Bus system bus access
- Services must be properly configured

### D-Bus Interface Tests
- Services must be running and registered on D-Bus
- D-Bus system bus access

### CLI Tool Tests
- CLI tools must be installed and in PATH
- Services must be running (for full functionality tests)

### Graceful Degradation Tests
- Services must be running
- Ability to simulate dependency failures

## Test Scenarios

### Service Communication
- Data Transmission Guard → Audit Service logging
- Update Service → Audit Service logging
- Event format consistency
- High-throughput logging

### D-Bus Interfaces
- All methods functional
- All signals emit correctly
- Error handling
- Security policies

### CLI Tools
- All commands functional
- 100% functional equivalence to D-Bus
- Error handling
- User-friendly messages

### Service Isolation
- systemd security features
- MAC policies (documented)
- Resource limits
- Privilege restrictions

### Graceful Degradation
- Network hooks unavailable (Data Guard)
- Database unavailable (Audit Service)
- Package management unavailable (Update Service)
- Service recovery

## Notes

- Many tests require services to be running in a test environment
- Some tests are skipped if services are not available
- Full integration testing requires systemd environment
- MAC policy tests require SELinux/AppArmor environment
- Performance tests should be run separately with appropriate tools

