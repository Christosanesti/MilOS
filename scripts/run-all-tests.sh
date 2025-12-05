#!/bin/bash
# Run all MilOS unit tests
# Handles both cases: with and without dependencies

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
TEST_BUILD_DIR="$PROJECT_ROOT/tests/build"

cd "$TEST_BUILD_DIR"

echo "=========================================="
echo "MilOS Test Suite Runner"
echo "=========================================="
echo ""

# Check for dependencies
HAS_YAML_CPP=false
HAS_LIBPCAP=false

if pkg-config --exists yaml-cpp 2>/dev/null; then
    HAS_YAML_CPP=true
    echo "✅ yaml-cpp: FOUND"
else
    echo "⚠️  yaml-cpp: NOT FOUND (some tests will be skipped)"
fi

if pkg-config --exists libpcap 2>/dev/null; then
    HAS_LIBPCAP=true
    echo "✅ libpcap: FOUND"
else
    echo "⚠️  libpcap: NOT FOUND (some tests will be skipped)"
fi

echo ""

# Find all test executables
TESTS=()
PASSED=0
FAILED=0
SKIPPED=0

echo "Running tests..."
echo "----------------------------------------"

# Test suites that should always be available
ALWAYS_TESTS=(
    "unit/test_dbus_harness"
    "unit/test_audit_logger"
    "unit/test_stig_checker"
)

# Test suites that require dependencies
CONDITIONAL_TESTS=(
    "unit/test_policy_manager:yaml-cpp"
    "unit/test_network_enforcement:yaml-cpp,libpcap"
    "unit/test_air_gap_manager:"
)

# Run always-available tests
for test in "${ALWAYS_TESTS[@]}"; do
    if [ -f "$TEST_BUILD_DIR/$test" ]; then
        echo ""
        echo "Running: $test"
        echo "----------------------------------------"
        if "$TEST_BUILD_DIR/$test" 2>&1 | tee /tmp/test_output.log; then
            RESULT=$(grep -E "Totals:.*passed" /tmp/test_output.log || echo "")
            if echo "$RESULT" | grep -q "passed"; then
                PASSED=$((PASSED + 1))
                echo "✅ PASSED"
            else
                FAILED=$((FAILED + 1))
                echo "❌ FAILED"
            fi
        else
            FAILED=$((FAILED + 1))
            echo "❌ FAILED (exit code: $?)"
        fi
    else
        SKIPPED=$((SKIPPED + 1))
        echo "⏭️  SKIPPED (not built)"
    fi
done

# Run conditional tests
for test_spec in "${CONDITIONAL_TESTS[@]}"; do
    IFS=':' read -r test deps <<< "$test_spec"
    if [ -f "$TEST_BUILD_DIR/$test" ]; then
        # Check dependencies
        CAN_RUN=true
        if [ -n "$deps" ]; then
            IFS=',' read -ra DEP_ARRAY <<< "$deps"
            for dep in "${DEP_ARRAY[@]}"; do
                case "$dep" in
                    "yaml-cpp")
                        if [ "$HAS_YAML_CPP" = false ]; then
                            CAN_RUN=false
                        fi
                        ;;
                    "libpcap")
                        if [ "$HAS_LIBPCAP" = false ]; then
                            CAN_RUN=false
                        fi
                        ;;
                esac
            done
        fi
        
        if [ "$CAN_RUN" = true ]; then
            echo ""
            echo "Running: $test"
            echo "----------------------------------------"
            if "$TEST_BUILD_DIR/$test" 2>&1 | tee /tmp/test_output.log; then
                RESULT=$(grep -E "Totals:.*passed" /tmp/test_output.log || echo "")
                if echo "$RESULT" | grep -q "passed"; then
                    PASSED=$((PASSED + 1))
                    echo "✅ PASSED"
                else
                    FAILED=$((FAILED + 1))
                    echo "❌ FAILED"
                fi
            else
                FAILED=$((FAILED + 1))
                echo "❌ FAILED (exit code: $?)"
            fi
        else
            SKIPPED=$((SKIPPED + 1))
            echo "⏭️  SKIPPED: $test (missing dependencies: $deps)"
        fi
    else
        SKIPPED=$((SKIPPED + 1))
        echo "⏭️  SKIPPED: $test (not built)"
    fi
done

# Summary
echo ""
echo "=========================================="
echo "Test Summary"
echo "=========================================="
echo "✅ Passed:  $PASSED"
echo "❌ Failed:  $FAILED"
echo "⏭️  Skipped: $SKIPPED"
echo ""

if [ $FAILED -eq 0 ]; then
    echo "🎉 All tests passed!"
    exit 0
else
    echo "⚠️  Some tests failed"
    exit 1
fi

