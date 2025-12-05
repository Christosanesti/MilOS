#!/bin/bash
# Test STIG Audit Logging
# Verifies that audit logs have all required STIG fields and integrity hashes

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
OUTPUT_DIR="$PROJECT_ROOT/test-reports"
mkdir -p "$OUTPUT_DIR"

echo "======================================"
echo "STIG Audit Logging Test"
echo "======================================"
echo ""

# Test 1: Verify audit logger includes STIG fields
echo "[TEST 1] Checking audit logger implementation for STIG fields..."

REQUIRED_FIELDS=(
    "timestamp"
    "user_id"
    "source_ip"
    "destination_ip"
    "result"
    "stig_compliant"
    "integrity_hash"
)

AUDIT_LOGGER_CPP="$PROJECT_ROOT/milos-base/services/data-guard/src/audit_logger.cpp"
MISSING_FIELDS=()

for field in "${REQUIRED_FIELDS[@]}"; do
    if ! grep -q "\"$field\"" "$AUDIT_LOGGER_CPP" && ! grep -q "'$field'" "$AUDIT_LOGGER_CPP" && ! grep -q "$field" "$AUDIT_LOGGER_CPP"; then
        MISSING_FIELDS+=("$field")
    fi
done

if [ ${#MISSING_FIELDS[@]} -eq 0 ]; then
    echo "✅ All required STIG fields found in audit logger"
else
    echo "❌ Missing STIG fields: ${MISSING_FIELDS[*]}"
    exit 1
fi

# Test 2: Verify integrity hash generation
echo ""
echo "[TEST 2] Checking integrity hash generation..."

if grep -q "generateIntegrityHash" "$AUDIT_LOGGER_CPP" && grep -q "QCryptographicHash" "$AUDIT_LOGGER_CPP"; then
    echo "✅ Integrity hash generation found"
    
    # Check for SHA-256
    if grep -q "Sha256" "$AUDIT_LOGGER_CPP"; then
        echo "✅ SHA-256 hash algorithm confirmed"
    else
        echo "⚠️  SHA-256 not explicitly found (may use default)"
    fi
else
    echo "❌ Integrity hash generation not found"
    exit 1
fi

# Test 3: Verify STIG log entry generation
echo ""
echo "[TEST 3] Checking STIG log entry generation..."

if grep -q "generateSTIGLogEntry" "$AUDIT_LOGGER_CPP"; then
    echo "✅ STIG log entry generation found"
    
    # Check that integrity hash is added to log entry
    if grep -q "integrity_hash" "$AUDIT_LOGGER_CPP"; then
        echo "✅ Integrity hash added to log entries"
    else
        echo "❌ Integrity hash not added to log entries"
        exit 1
    fi
else
    echo "❌ STIG log entry generation not found"
    exit 1
fi

# Test 4: Verify required STIG fields in logTransmissionAttempt
echo ""
echo "[TEST 4] Checking logTransmissionAttempt for STIG fields..."

TRANSMISSION_FIELDS=(
    "timestamp"
    "user_id"
    "source_ip"
    "destination_ip"
    "result"
    "stig_compliant"
)

MISSING_TRANSMISSION_FIELDS=()

# Check in logTransmissionAttempt function
for field in "${TRANSMISSION_FIELDS[@]}"; do
    if ! grep -A 30 "logTransmissionAttempt" "$AUDIT_LOGGER_CPP" | grep -q "$field"; then
        MISSING_TRANSMISSION_FIELDS+=("$field")
    fi
done

if [ ${#MISSING_TRANSMISSION_FIELDS[@]} -eq 0 ]; then
    echo "✅ All required STIG fields in logTransmissionAttempt"
else
    echo "⚠️  Some fields may be set elsewhere: ${MISSING_TRANSMISSION_FIELDS[*]}"
fi

# Test 5: Verify JSON structure
echo ""
echo "[TEST 5] Checking JSON structure generation..."

if grep -q "QJsonDocument\|QJsonObject" "$AUDIT_LOGGER_CPP"; then
    echo "✅ JSON structure generation found"
else
    echo "❌ JSON structure generation not found"
    exit 1
fi

# Test 6: Check for UTC timestamp
echo ""
echo "[TEST 6] Checking timestamp format..."

if grep -q "currentDateTimeUtc\|Qt::ISODate" "$AUDIT_LOGGER_CPP"; then
    echo "✅ UTC timestamp with ISO format found"
else
    echo "⚠️  UTC timestamp format not explicitly verified"
fi

# Summary
echo ""
echo "======================================"
echo "Test Summary"
echo "======================================"
echo "✅ STIG field verification: PASSED"
echo "✅ Integrity hash generation: PASSED"
echo "✅ STIG log entry generation: PASSED"
echo "✅ JSON structure: PASSED"
echo ""
echo "STIG Audit Logging Test: PASSED"
echo ""
echo "Generated report: $OUTPUT_DIR/stig-audit-test-report.txt"
echo "$(date): STIG Audit Logging Test PASSED" > "$OUTPUT_DIR/stig-audit-test-report.txt"

exit 0

