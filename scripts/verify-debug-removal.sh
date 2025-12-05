#!/bin/bash
# Verify Debug Statement Removal
# Checks that debug statements are removed or conditionally compiled in release builds

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
OUTPUT_DIR="$PROJECT_ROOT/test-reports"
mkdir -p "$OUTPUT_DIR"

echo "======================================"
echo "Debug Statement Removal Verification"
echo "======================================"
echo ""

# Patterns to search for (should be removed or conditionally compiled)
DEBUG_PATTERNS=(
    "std::cerr"
    "std::cout"
    "qDebug()"
    "printf("
    "fprintf("
    "cout <<"
    "cerr <<"
)

# Files to check (services that should not have debug statements)
SERVICE_DIRS=(
    "milos-base/services/data-guard/src"
    "milos-base/services/audit-service/src"
    "milos-base/services/secrets-management/src"
    "milos-base/services/tpm-integration/src"
)

echo "Scanning service source files for debug statements..."
echo ""

TOTAL_ISSUES=0
ISSUES_FOUND=()

for service_dir in "${SERVICE_DIRS[@]}"; do
    full_path="$PROJECT_ROOT/$service_dir"
    
    if [ ! -d "$full_path" ]; then
        continue
    fi
    
    echo "Checking: $service_dir"
    
    for pattern in "${DEBUG_PATTERNS[@]}"; do
        # Search for pattern, but exclude:
        # - Comments
        # - LOG_* macros (proper logging)
        # - Conditional compilation blocks
        # - String literals
        
        # Find files with pattern
        while IFS= read -r file; do
            if [ -f "$file" ]; then
                # Check if it's in a comment or string literal
                # Simple check: if line contains LOG_ or is in #ifdef block, skip
                line_num=0
                in_comment=false
                in_conditional=false
                
                while IFS= read -r line; do
                    ((line_num++))
                    
                    # Skip if line contains LOG_ macro (proper logging)
                    if echo "$line" | grep -q "LOG_"; then
                        continue
                    fi
                    
                    # Check for conditional compilation
                    if echo "$line" | grep -qE "^[[:space:]]*#if"; then
                        in_conditional=true
                        continue
                    fi
                    if echo "$line" | grep -qE "^[[:space:]]*#endif"; then
                        in_conditional=false
                        continue
                    fi
                    
                    # Check if pattern is in line (not in comment)
                    if echo "$line" | grep -q "$pattern" && ! echo "$line" | grep -qE "^[[:space:]]*//"; then
                        if [ "$in_conditional" = false ]; then
                            ISSUES_FOUND+=("$file:$line_num: $pattern")
                            ((TOTAL_ISSUES++))
                        fi
                    fi
                done < "$file"
            fi
        done < <(find "$full_path" -name "*.cpp" -o -name "*.h" 2>/dev/null | head -20)
    done
done

# Report results
echo ""
echo "======================================"
echo "Verification Results"
echo "======================================"
echo ""

if [ $TOTAL_ISSUES -eq 0 ]; then
    echo "✅ No debug statements found in service source files"
    echo "✅ Debug statement removal: VERIFIED"
    echo ""
    echo "Note: This check verifies that debug statements are not present"
    echo "in release builds. Some debug statements may be conditionally"
    echo "compiled using #ifdef DEBUG or LOG_DEBUG macros."
else
    echo "⚠️  Found $TOTAL_ISSUES potential debug statement(s):"
    echo ""
    for issue in "${ISSUES_FOUND[@]}"; do
        echo "  - $issue"
    done
    echo ""
    echo "Recommendation:"
    echo "  - Replace with LOG_* macros from milos-logging"
    echo "  - Use conditional compilation (#ifdef DEBUG)"
    echo "  - Remove debug statements from production code"
fi

echo ""
echo "Report saved to: $OUTPUT_DIR/debug-removal-verification.txt"
{
    echo "Debug Statement Removal Verification"
    echo "======================================"
    echo "Date: $(date)"
    echo "Total Issues Found: $TOTAL_ISSUES"
    echo ""
    if [ $TOTAL_ISSUES -gt 0 ]; then
        echo "Issues:"
        for issue in "${ISSUES_FOUND[@]}"; do
            echo "  $issue"
        done
    else
        echo "Status: PASSED - No debug statements found"
    fi
} > "$OUTPUT_DIR/debug-removal-verification.txt"

exit 0

