#!/bin/bash
# Prepare code coverage generation
# Run this after installing dependencies and running tests

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
TEST_BUILD_DIR="$PROJECT_ROOT/tests/build"

echo "=== Preparing Code Coverage Generation ==="
echo ""

# Check if test build directory exists
if [ ! -d "$TEST_BUILD_DIR" ]; then
    echo "❌ Test build directory not found: $TEST_BUILD_DIR"
    echo "   Please build tests first:"
    echo "   cd tests && mkdir -p build && cd build"
    echo "   cmake -DENABLE_COVERAGE=ON .."
    echo "   make -j\$(nproc)"
    exit 1
fi

# Check if coverage is enabled
if [ ! -f "$TEST_BUILD_DIR/CMakeCache.txt" ]; then
    echo "⚠️  CMakeCache.txt not found. Rebuilding with coverage..."
    cd "$TEST_BUILD_DIR"
    cmake -DENABLE_COVERAGE=ON ..
fi

# Check if gcov is available
if ! command -v gcov &> /dev/null; then
    echo "❌ gcov not found. Install: sudo apt-get install -y gcov"
    exit 1
fi

# Check if lcov is available
if ! command -v lcov &> /dev/null; then
    echo "❌ lcov not found. Install: sudo apt-get install -y lcov"
    exit 1
fi

echo "✅ Coverage tools available"
echo "✅ Test build directory found"
echo ""
echo "To generate coverage report:"
echo "  1. Run all tests: cd $TEST_BUILD_DIR && make test"
echo "  2. Generate coverage: cd $TEST_BUILD_DIR && make coverage"
echo "  3. View report: firefox $TEST_BUILD_DIR/coverage/index.html"
echo ""
