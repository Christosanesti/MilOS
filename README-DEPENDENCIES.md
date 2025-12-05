# Dependency Installation Guide

## Quick Install

Run these commands in your terminal:

```bash
sudo apt-get update
sudo apt-get install -y libyaml-cpp-dev libpcap-dev
```

## Verify Installation

After installation, verify dependencies are available:

```bash
./scripts/verify-and-install-deps.sh
```

This script will check for dependencies and tell you if they're installed.

## What These Dependencies Enable

- **libyaml-cpp-dev**: Enables `test_policy_manager` (~20 tests)
- **libpcap-dev**: Enables `test_network_enforcement` (~20 tests)

**Total:** Unlocks ~40 additional tests

## After Installation

Once dependencies are installed:

1. **Rebuild tests:**
   ```bash
   cd tests/build
   rm -rf *
   cmake -DENABLE_COVERAGE=ON ..
   make -j$(nproc)
   ```

2. **Run all tests:**
   ```bash
   cd ../..
   ./scripts/run-all-tests.sh
   ```

## Current Status

- ✅ 36 tests passing (3 test suites)
- ⏳ ~40 tests ready (2 test suites awaiting dependencies)

**After installation:** 70+ tests will be available!

---

**Need help?** See `INSTALL-DEPENDENCIES.md` for detailed instructions.

