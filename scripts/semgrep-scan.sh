#!/bin/bash
# Semgrep Security Scanning Script
# Performs Static Analysis Security Testing (SAST) on MilOS codebase

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
OUTPUT_DIR="$PROJECT_ROOT/semgrep-reports"
mkdir -p "$OUTPUT_DIR"

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${GREEN}MilOS Semgrep Security Scan${NC}"
echo "===================================="
echo ""

# Check if semgrep is installed
if ! command -v semgrep &> /dev/null; then
    echo -e "${YELLOW}Warning: semgrep not found. Attempting to install...${NC}"
    
    # Try to install via pip
    if command -v pip3 &> /dev/null; then
        echo "Installing semgrep via pip3..."
        pip3 install semgrep --user
        export PATH="$HOME/.local/bin:$PATH"
    elif command -v pip &> /dev/null; then
        echo "Installing semgrep via pip..."
        pip install semgrep --user
        export PATH="$HOME/.local/bin:$PATH"
    else
        echo -e "${RED}Error: pip not found. Please install semgrep manually:${NC}"
        echo "  pip install semgrep"
        echo ""
        echo "Or download from: https://github.com/returntocorp/semgrep"
        exit 1
    fi
    
    # Verify installation
    if ! command -v semgrep &> /dev/null; then
        echo -e "${RED}Error: semgrep installation failed${NC}"
        exit 1
    fi
fi

echo "Project Root: $PROJECT_ROOT"
echo "Output Directory: $OUTPUT_DIR"
echo ""

# Update Semgrep rules
echo -e "${GREEN}Updating Semgrep rules...${NC}"
semgrep --update || echo "Warning: Rule update failed, continuing with existing rules"

# Scan types
SCAN_TYPES=(
    "security"      # Security-focused rules
    "csharp"        # C# rules
    "cpp"           # C++ rules
    "python"        # Python rules
    "javascript"    # JavaScript rules
    "typescript"    # TypeScript rules
)

# Scan the codebase
echo ""
echo -e "${GREEN}Scanning codebase for security issues...${NC}"

# Main security scan
echo "Running security scan..."
semgrep --config=auto \
        --json \
        --output="$OUTPUT_DIR/semgrep-security-scan.json" \
        "$PROJECT_ROOT/milos-base" \
        "$PROJECT_ROOT/milos-gui-tools" \
        2>&1 | tee "$OUTPUT_DIR/semgrep-security-scan.log" || true

# C++ specific scan
echo "Running C++ security scan..."
semgrep --config=auto \
        --lang=cpp \
        --json \
        --output="$OUTPUT_DIR/semgrep-cpp-scan.json" \
        "$PROJECT_ROOT/milos-base" \
        2>&1 | tee "$OUTPUT_DIR/semgrep-cpp-scan.log" || true

# Python specific scan
if [ -d "$PROJECT_ROOT/milos-base/services" ]; then
    echo "Running Python security scan..."
    find "$PROJECT_ROOT/milos-base" -name "*.py" | head -1 > /dev/null 2>&1 && \
    semgrep --config=auto \
            --lang=python \
            --json \
            --output="$OUTPUT_DIR/semgrep-python-scan.json" \
            "$PROJECT_ROOT/milos-base" \
            2>&1 | tee "$OUTPUT_DIR/semgrep-python-scan.log" || true
fi

# Generate human-readable report
echo ""
echo -e "${GREEN}Generating human-readable report...${NC}"

# Parse JSON and generate summary
if [ -f "$OUTPUT_DIR/semgrep-security-scan.json" ]; then
    # Count findings by severity
    CRITICAL=$(jq '[.results[] | select(.extra.severity == "ERROR")] | length' "$OUTPUT_DIR/semgrep-security-scan.json" 2>/dev/null || echo "0")
    HIGH=$(jq '[.results[] | select(.extra.severity == "WARNING")] | length' "$OUTPUT_DIR/semgrep-security-scan.json" 2>/dev/null || echo "0")
    MEDIUM=$(jq '[.results[] | select(.extra.severity == "INFO")] | length' "$OUTPUT_DIR/semgrep-security-scan.json" 2>/dev/null || echo "0")
    
    # Generate text report
    {
        echo "Semgrep Security Scan Report"
        echo "============================"
        echo ""
        echo "Generated: $(date)"
        echo "Project: MilOS"
        echo ""
        echo "Vulnerability Summary:"
        echo "  CRITICAL: $CRITICAL"
        echo "  HIGH: $HIGH"
        echo "  MEDIUM: $MEDIUM"
        echo ""
        echo "Detailed findings available in:"
        echo "  - $OUTPUT_DIR/semgrep-security-scan.json"
        echo "  - $OUTPUT_DIR/semgrep-cpp-scan.json"
        echo ""
    } > "$OUTPUT_DIR/semgrep-scan-summary.txt"
    
    # Extract top findings
    if [ "$CRITICAL" != "0" ] || [ "$HIGH" != "0" ]; then
        echo "Top Security Findings:" >> "$OUTPUT_DIR/semgrep-scan-summary.txt"
        echo "" >> "$OUTPUT_DIR/semgrep-scan-summary.txt"
        
        # Extract ERROR and WARNING findings
        jq -r '.results[] | select(.extra.severity == "ERROR" or .extra.severity == "WARNING") | "\(.extra.severity): \(.check_id) - \(.message)\n  File: \(.path):\(.start.line)\n"' "$OUTPUT_DIR/semgrep-security-scan.json" 2>/dev/null | head -50 >> "$OUTPUT_DIR/semgrep-scan-summary.txt" || true
    fi
    
    cat "$OUTPUT_DIR/semgrep-scan-summary.txt"
fi

echo ""
echo -e "${GREEN}Scan Complete${NC}"
echo "============="
echo "Output directory: $OUTPUT_DIR"
echo ""
echo "Generated reports:"
echo "  $OUTPUT_DIR/semgrep-security-scan.json"
echo "  $OUTPUT_DIR/semgrep-cpp-scan.json"
echo "  $OUTPUT_DIR/semgrep-scan-summary.txt"
echo ""
echo -e "${GREEN}Next Steps:${NC}"
echo "1. Review vulnerability reports"
echo "2. Address critical and high-severity findings"
echo "3. Integrate into CI/CD pipeline"

exit 0

