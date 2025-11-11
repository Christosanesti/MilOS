# Data Transmission Guard Service Tests

This directory contains unit tests, integration tests, and security tests for the Data Transmission Guard service.

## Test Structure

### Unit Tests

- **test_policy_manager.cpp**: Tests for policy management, validation, and application
- **test_config_parser.cpp**: Tests for YAML configuration parsing
- **test_network_enforcement.cpp**: Tests for network packet inspection and encryption detection

### Integration Tests

Integration tests verify the interaction between components and external services:
- D-Bus interface integration
- Audit service integration
- Network stack integration

### Security Tests

Security tests verify policy enforcement and service isolation:
- Policy enforcement verification
- Service isolation tests (systemd + MAC policies)
- Graceful degradation tests

## Running Tests

### Build Tests

```bash
cd milos-base/services/data-guard
mkdir build && cd build
cmake ..
make
```

### Run All Tests

```bash
ctest
```

### Run Specific Test

```bash
./test_policy_manager
./test_config_parser
./test_network_enforcement
```

### Run with Verbose Output

```bash
ctest -V
```

## Test Coverage

- **Policy Management**: Policy loading, validation, application, priority sorting
- **Configuration Parsing**: YAML parsing, nested key access, default values
- **Network Enforcement**: Packet inspection, encryption detection, policy application
- **D-Bus Interface**: Method calls, signal emission, service registration
- **Health Monitoring**: Health checks, watchdog updates, status reporting

## Notes

- Some tests require root privileges for network hook testing
- Integration tests require D-Bus system bus access
- Security tests may require systemd and MAC policy configuration

