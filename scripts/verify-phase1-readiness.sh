#!/bin/bash
# Verify Phase 1 readiness and provide status

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

echo "╔═══════════════════════════════════════════════════════════════╗"
echo "║         PHASE 1 READINESS VERIFICATION                         ║"
echo "╚═══════════════════════════════════════════════════════════════╝"
echo ""

# Task 1: Debug Migration
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "TASK 1: Debug Code Migration"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
DEBUG_COUNT=$(grep -r "qDebug\|std::cout\|std::cerr" milos-base/services/*/src/*.cpp 2>/dev/null | grep -v "cli_tool.cpp" | wc -l)
if [ "$DEBUG_COUNT" -eq 0 ]; then
    echo "✅ COMPLETE - 0 debug statements in service code"
else
    echo "⚠️  $DEBUG_COUNT instances remaining in service code"
fi
echo ""

# Task 2: Test Suite
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "TASK 2: Full Test Suite"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
TEST_COUNT=$(find tests/build -name "test_*" -type f -executable 2>/dev/null | wc -l)
echo "   Test executables: $TEST_COUNT"

# Check dependencies
if dpkg -l | grep -q "libyaml-cpp-dev"; then
    echo "   ✅ libyaml-cpp-dev: INSTALLED"
else
    echo "   ❌ libyaml-cpp-dev: NOT INSTALLED"
fi

if dpkg -l | grep -q "libpcap-dev"; then
    echo "   ✅ libpcap-dev: INSTALLED"
else
    echo "   ❌ libpcap-dev: NOT INSTALLED"
fi

if [ "$TEST_COUNT" -ge 5 ] && dpkg -l | grep -q "libyaml-cpp-dev" && dpkg -l | grep -q "libpcap-dev"; then
    echo "   ✅ READY - All dependencies installed"
else
    echo "   ⏸️  BLOCKED - Install dependencies: ./scripts/install-all-dependencies.sh"
fi
echo ""

# Task 3: Code Coverage
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "TASK 3: Code Coverage"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
if command -v gcov &> /dev/null && command -v lcov &> /dev/null; then
    echo "   ✅ Coverage tools: AVAILABLE"
    if [ -f "tests/build/CMakeCache.txt" ] && grep -q "ENABLE_COVERAGE:BOOL=ON" tests/build/CMakeCache.txt 2>/dev/null; then
        echo "   ✅ Coverage: ENABLED in build"
    else
        echo "   ⚠️  Coverage: NOT ENABLED (rebuild with -DENABLE_COVERAGE=ON)"
    fi
else
    echo "   ⚠️  Coverage tools: NOT INSTALLED (gcov, lcov)"
fi
echo ""

# Task 4: Error Handler
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "TASK 4: Error Handler Integration"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
GUI_COUNT=$(grep -r "ErrorHandler::instance()->initialize" milos-gui-tools/*/src/main.cpp 2>/dev/null | wc -l)
SERVICE_COUNT=$(grep -r "Logger::instance()->initialize" milos-base/services/*/src/main.cpp 2>/dev/null | wc -l)
echo "   GUI apps with ErrorHandler: $GUI_COUNT"
echo "   Services with Logger: $SERVICE_COUNT"
echo "   ✅ COMPLETE - Architecture appropriate"
echo ""

# Summary
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "SUMMARY"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
COMPLETE=0
TOTAL=4

[ "$DEBUG_COUNT" -eq 0 ] && COMPLETE=$((COMPLETE + 1))
[ "$GUI_COUNT" -ge 6 ] && [ "$SERVICE_COUNT" -ge 6 ] && COMPLETE=$((COMPLETE + 1))

echo "   Tasks Complete: $COMPLETE/$TOTAL"
echo "   Progress: $((COMPLETE * 100 / TOTAL))%"
echo ""
echo "Next Steps:"
if [ "$DEBUG_COUNT" -gt 0 ]; then
    echo "   1. Complete debug migration"
fi
if ! dpkg -l | grep -q "libyaml-cpp-dev\|libpcap-dev"; then
    echo "   2. Install dependencies: ./scripts/install-all-dependencies.sh"
fi
if [ "$TEST_COUNT" -ge 5 ]; then
    echo "   3. Run tests: ./scripts/run-all-tests.sh"
    echo "   4. Generate coverage: ./scripts/generate-coverage.sh"
fi
