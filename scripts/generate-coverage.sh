#!/bin/bash
# Generate code coverage report for MilOS
# Prerequisites: Dependencies installed, tests built with coverage enabled

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
TEST_BUILD_DIR="$PROJECT_ROOT/tests/build"

echo "=== MilOS Code Coverage Generation ==="
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

cd "$TEST_BUILD_DIR"

# Check if coverage is enabled
if ! grep -q "ENABLE_COVERAGE:BOOL=ON" CMakeCache.txt 2>/dev/null; then
    echo "⚠️  Coverage not enabled in build. Rebuilding with coverage..."
    cmake -DENABLE_COVERAGE=ON ..
    make -j$(nproc)
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

# Check if genhtml is available
if ! command -v genhtml &> /dev/null; then
    echo "❌ genhtml not found. Install: sudo apt-get install -y lcov"
    exit 1
fi

echo "✅ Coverage tools available"
echo "✅ Test build directory found"
echo ""

# Run tests first to generate coverage data
echo "Running tests to generate coverage data..."
if [ -f Makefile ]; then
    make test || echo "⚠️  Some tests may have failed, continuing with coverage generation..."
else
    echo "❌ Makefile not found. Please build tests first."
    exit 1
fi

echo ""
echo "Generating coverage report..."

# Generate coverage report
if [ -f Makefile ] && grep -q "coverage:" Makefile; then
    make coverage
    echo ""
    echo "✅ Coverage report generated!"
    echo "   Location: $TEST_BUILD_DIR/coverage/index.html"
    echo ""
    echo "To view the report:"
    echo "   firefox $TEST_BUILD_DIR/coverage/index.html"
    echo "   OR"
    echo "   xdg-open $TEST_BUILD_DIR/coverage/index.html"
else
    echo "❌ Coverage target not found in Makefile"
    echo "   Ensure ENABLE_COVERAGE=ON was set during cmake configuration"
    exit 1
fi
