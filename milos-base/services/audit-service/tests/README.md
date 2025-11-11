# Audit Service Test Suite

This directory contains unit tests for the MilOS Audit Service.

## Test Structure

The test suite is organized by component:

- **test_log_storage.cpp** - Tests for log storage operations (SQLite database)
- **test_hash_chain.cpp** - Tests for hash chain generation and verification
- **test_event_collector.cpp** - Tests for event collection and validation
- **test_query_engine.cpp** - Tests for query parsing, filtering, and export
- **test_config_parser.cpp** - Tests for YAML configuration parsing

## Building Tests

Tests are built as part of the main CMake build:

```bash
cd milos-base/services/audit-service
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
./tests/test_log_storage
./tests/test_hash_chain
./tests/test_event_collector
./tests/test_query_engine
./tests/test_config_parser
```

### Run with verbose output:

```bash
cd build
ctest --verbose
```

## Test Coverage

### Log Storage Tests
- Database initialization
- Log entry storage
- Query operations with filters
- Event statistics
- Retention policy enforcement
- Last log hash retrieval
- Health status checking

### Hash Chain Tests
- Hash chain initialization
- Hash generation
- Hash chain consistency
- Integrity verification
- Integrity violation detection
- Initialization vector generation

### Event Collector Tests
- Event collection
- Event validation
- Event queuing
- Batch processing
- Health status checking

### Query Engine Tests
- Query parsing
- Filtering operations
- Statistics calculation
- Export to JSON
- Export to CSV
- Export to XML

### Config Parser Tests
- YAML file loading
- Configuration value access (string, bool, int)
- Nested key access
- Default value handling
- Error handling for invalid/missing configs

## Integration Tests

Integration tests require:
- D-Bus system bus access
- SQLite database access
- Systemd environment (for service isolation tests)
- Linux audit framework (auditd) for auditd integration tests

Integration tests are not included in the unit test suite and should be run separately in a test environment.

## Performance Tests

Performance tests for log throughput and query performance are not included in the unit test suite. These should be run separately with appropriate benchmarking tools.

## Security Tests

Security tests for tamper-proof verification and integrity checking are partially covered in the hash chain tests. Additional security tests should be run separately to verify:
- Tamper detection
- Hash chain integrity under various attack scenarios
- Log retention policy enforcement
- Database security

## Notes

- Tests use temporary directories for database and configuration files
- Tests clean up after themselves
- Some tests require Qt event loop for async operations (event collector)
- D-Bus tests require D-Bus system bus access (may require root or proper permissions)

