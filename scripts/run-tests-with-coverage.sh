#!/bin/bash
# Run full test suite with code coverage

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
TEST_BUILD_DIR="$PROJECT_ROOT/tests/build"

echo "=== MilOS Test Suite with Coverage ==="
echo ""

# Check if dependencies are installed
echo "Checking dependencies..."
MISSING_DEPS=0

if ! dpkg -l | grep -q libyaml-cpp-dev; then
    echo "❌ libyaml-cpp-dev not installed"
    MISSING_DEPS=$((MISSING_DEPS + 1))
else
    echo "✅ libyaml-cpp-dev installed"
fi

if ! dpkg -l | grep -q libpcap-dev; then
    echo "❌ libpcap-dev not installed"
    MISSING_DEPS=$((MISSING_DEPS + 1))
else
    echo "✅ libpcap-dev installed"
fi

if [ $MISSING_DEPS -gt 0 ]; then
    echo ""
    echo "⚠️  Missing dependencies. Please install:"
    echo "   ./scripts/install-all-dependencies.sh"
    exit 1
fi

# Check for coverage tools
echo ""
echo "Checking coverage tools..."
if ! command -v gcov &> /dev/null; then
    echo "⚠️  gcov not found (usually part of gcc)"
else
    echo "✅ gcov found"
fi

if ! command -v lcov &> /dev/null; then
    echo "⚠️  lcov not found (install: sudo apt-get install lcov)"
else
    echo "✅ lcov found"
fi

if ! command -v genhtml &> /dev/null; then
    echo "⚠️  genhtml not found (usually part of lcov)"
else
    echo "✅ genhtml found"
fi

# Build tests with coverage
echo ""
echo "Building tests with coverage enabled..."
cd "$TEST_BUILD_DIR"
rm -rf *
cmake -DENABLE_COVERAGE=ON ..
make -j$(nproc)

# Run tests
echo ""
echo "Running test suite..."
ctest --output-on-failure

# Generate coverage report
echo ""
echo "Generating coverage report..."
make coverage

echo ""
echo "=== Coverage Report Generated ==="
echo "View report: $TEST_BUILD_DIR/coverage/index.html"
echo ""
