# Update Service Test Suite

This directory contains unit tests for the MilOS Update Service.

## Test Structure

The test suite is organized by component:

- **test_config_parser.cpp** - Tests for YAML configuration parsing

## Building Tests

Tests are built as part of the main CMake build:

```bash
cd milos-base/services/update-service
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
./tests/test_config_parser
```

## Test Coverage

### Config Parser Tests
- YAML file loading
- Configuration value access (string, bool, int)
- Nested key access
- Default value handling
- Error handling for invalid/missing configs

## Integration Tests

Integration tests require:
- D-Bus system bus access
- Package manager (pacman or APT) environment
- GPG keyring for signature verification
- Systemd environment (for service isolation tests)
- Audit Service for audit logging integration tests

Integration tests are not included in the unit test suite and should be run separately in a test environment.

## Performance Tests

Performance tests for update application throughput are not included in the unit test suite. These should be run separately with appropriate benchmarking tools.

## Security Tests

Security tests for signature verification and rollback verification are not included in the unit test suite. These should be run separately to verify:
- GPG signature verification
- Rollback integrity
- Package integrity verification

## Notes

- Tests use temporary directories for configuration files
- Tests clean up after themselves
- Package manager tests require actual package manager environment (pacman/APT)
- Signature verification tests require GPG keyring and signed packages
- D-Bus tests require D-Bus system bus access (may require root or proper permissions)

