# MilOS Testing Guide

**Quick Start:** Run `./scripts/run-all-tests.sh` to execute all available tests.

---

## Overview

MilOS has a comprehensive test suite with 70+ unit tests covering:
- D-Bus communication and mocking
- Audit logging and STIG compliance
- Policy management
- Network enforcement
- STIG compliance checking
- Air-gap deployment management

---

## Prerequisites

### Required Dependencies

For the full test suite:
```bash
sudo apt-get install -y libyaml-cpp-dev libpcap-dev
```

Or use the installation script:
```bash
./scripts/install-test-dependencies.sh
```

### Optional (for coverage reports)
```bash
sudo apt-get install -y lcov
```

---

## Running Tests

### Quick Start

**Run all available tests:**
```bash
./scripts/run-all-tests.sh
```

**Build and run manually:**
```bash
cd tests/build
rm -rf *
cmake -DENABLE_COVERAGE=ON ..
make -j$(nproc)
find unit -name "test_*" -type f -executable -exec {} \;
```

### Individual Test Suites

```bash
cd tests/build

# Always available
./unit/test_dbus_harness
./unit/test_audit_logger
./unit/test_stig_checker

# Requires dependencies
./unit/test_policy_manager      # needs yaml-cpp
./unit/test_network_enforcement # needs yaml-cpp, libpcap
./unit/test_air_gap_manager
```

---

## Test Coverage

### Current Status
- ✅ 36 tests passing (3 test suites)
- ⚠️ ~40 tests blocked (require dependencies)
- 📊 Coverage: Ready to generate (needs lcov)

### Generate Coverage Report

```bash
cd tests/build
rm -rf *
cmake -DENABLE_COVERAGE=ON ..
make -j$(nproc)

# Run tests
./scripts/run-all-tests.sh

# Generate report
make coverage

# View report
xdg-open coverage/index.html
```

---

## Test Structure

```
tests/
├── unit/              # Unit tests
│   ├── test_*.cpp     # Test files
│   └── CMakeLists.txt # Test configuration
├── harnesses/         # Test harnesses
│   ├── dbus_test_harness.*  # D-Bus mocking
│   └── CMakeLists.txt
├── integration/       # Integration tests (future)
├── performance/       # Performance tests
└── build/             # Build directory
```

---

## Troubleshooting

### Tests Won't Compile

**Missing dependencies:**
```bash
./scripts/install-test-dependencies.sh
```

**CMake errors:**
```bash
cd tests/build
rm -rf *
cmake -DENABLE_COVERAGE=ON ..
```

### Tests Fail

**Check test output:**
```bash
./unit/test_* --verbose
```

**Check dependencies:**
```bash
pkg-config --exists yaml-cpp && echo "OK" || echo "MISSING"
pkg-config --exists libpcap && echo "OK" || echo "MISSING"
```

### Coverage Not Generating

**Install lcov:**
```bash
sudo apt-get install -y lcov
```

**Rebuild with coverage:**
```bash
cd tests/build
rm -rf *
cmake -DENABLE_COVERAGE=ON ..
make -j$(nproc)
```

---

## Documentation

- `docs/TEST-STATUS-SUMMARY.md` - Current test status
- `docs/DEPENDENCY-INSTALLATION.md` - Installation guide
- `docs/FINAL-TEST-STATUS-REPORT.md` - Comprehensive report
- `docs/NEXT-STEPS-ROADMAP.md` - Future roadmap

---

## Scripts

- `scripts/install-test-dependencies.sh` - Install dependencies
- `scripts/run-all-tests.sh` - Run all tests with smart dependency detection

---

**Status:** ✅ Test infrastructure ready - Install dependencies to unlock full suite

