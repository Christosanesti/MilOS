#!/bin/bash
#
# Trivy Vulnerability Scanning Script for MilOS
# Scans codebase, dependencies, and filesystem for vulnerabilities
# Required for NIST SP 800-161 compliance
#
# Usage: ./trivy-scan.sh [scan-type] [output-dir]
#   scan-type: all|fs|repo|config (default: all)
#

set -e

SCAN_TYPE="${1:-all}"
OUTPUT_DIR="${2:-./trivy-reports}"
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TRIVY_CMD="trivy"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}MilOS Trivy Vulnerability Scan${NC}"
echo "===================================="
echo ""

# Check if trivy is installed
if ! command -v "$TRIVY_CMD" &> /dev/null; then
    echo -e "${YELLOW}Warning: trivy not found. Attempting to download...${NC}"
    
    # Try to download trivy
    TRIVY_VERSION="0.51.0"
    TRIVY_URL="https://github.com/aquasecurity/trivy/releases/download/v${TRIVY_VERSION}/trivy_${TRIVY_VERSION}_Linux-64bit.tar.gz"
    
    if command -v wget &> /dev/null; then
        wget -q "$TRIVY_URL" -O /tmp/trivy.tar.gz
        tar -xzf /tmp/trivy.tar.gz -C /tmp trivy
        chmod +x /tmp/trivy
        TRIVY_CMD="/tmp/trivy"
    elif command -v curl &> /dev/null; then
        curl -L "$TRIVY_URL" -o /tmp/trivy.tar.gz
        tar -xzf /tmp/trivy.tar.gz -C /tmp trivy
        chmod +x /tmp/trivy
        TRIVY_CMD="/tmp/trivy"
    else
        echo -e "${RED}Error: trivy not found and cannot download (wget/curl not available)${NC}"
        echo "Please install trivy manually:"
        echo "  wget https://github.com/aquasecurity/trivy/releases/latest/download/trivy_X.X.X_Linux-64bit.tar.gz"
        echo "  tar -xzf trivy_X.X.X_Linux-64bit.tar.gz"
        echo "  sudo mv trivy /usr/local/bin/"
        exit 1
    fi
fi

# Create output directory
mkdir -p "$OUTPUT_DIR"

echo "Project Root: $PROJECT_ROOT"
echo "Output Directory: $OUTPUT_DIR"
echo "Scan Type: $SCAN_TYPE"
echo ""

# Update trivy database
echo -e "${GREEN}Updating Trivy vulnerability database...${NC}"
"$TRIVY_CMD" image --download-db-only || {
    echo -e "${YELLOW}Warning: Database update failed, continuing with existing database${NC}"
}

# Filesystem scan
if [ "$SCAN_TYPE" = "all" ] || [ "$SCAN_TYPE" = "fs" ]; then
    echo ""
    echo -e "${GREEN}Scanning filesystem for vulnerabilities...${NC}"
    "$TRIVY_CMD" fs \
        --format json \
        --output "$OUTPUT_DIR/trivy-fs-scan.json" \
        --severity CRITICAL,HIGH,MEDIUM,LOW \
        "$PROJECT_ROOT" || {
        echo -e "${YELLOW}Warning: Filesystem scan failed${NC}"
    }
    
    # Also generate human-readable report
    "$TRIVY_CMD" fs \
        --format table \
        --output "$OUTPUT_DIR/trivy-fs-scan.txt" \
        --severity CRITICAL,HIGH \
        "$PROJECT_ROOT" || true
fi

# Repository scan (for dependency vulnerabilities)
if [ "$SCAN_TYPE" = "all" ] || [ "$SCAN_TYPE" = "repo" ]; then
    echo ""
    echo -e "${GREEN}Scanning repository for dependency vulnerabilities...${NC}"
    
    # Scan CMakeLists.txt files for dependencies
    if [ -f "$PROJECT_ROOT/CMakeLists.txt" ]; then
        "$TRIVY_CMD" fs \
            --format json \
            --output "$OUTPUT_DIR/trivy-repo-scan.json" \
            --severity CRITICAL,HIGH,MEDIUM,LOW \
            "$PROJECT_ROOT" || {
            echo -e "${YELLOW}Warning: Repository scan failed${NC}"
        }
    fi
fi

# Configuration scan
if [ "$SCAN_TYPE" = "all" ] || [ "$SCAN_TYPE" = "config" ]; then
    echo ""
    echo -e "${GREEN}Scanning configuration files for misconfigurations...${NC}"
    "$TRIVY_CMD" config \
        --format json \
        --output "$OUTPUT_DIR/trivy-config-scan.json" \
        --severity CRITICAL,HIGH,MEDIUM \
        "$PROJECT_ROOT" || {
        echo -e "${YELLOW}Warning: Configuration scan failed${NC}"
    }
fi

# Summary report
echo ""
echo -e "${GREEN}Generating summary report...${NC}"

if [ -f "$OUTPUT_DIR/trivy-fs-scan.json" ]; then
    if command -v jq &> /dev/null; then
        critical=$(jq '[.Results[]?.Vulnerabilities[]? | select(.Severity == "CRITICAL")] | length' "$OUTPUT_DIR/trivy-fs-scan.json" 2>/dev/null || echo "0")
        high=$(jq '[.Results[]?.Vulnerabilities[]? | select(.Severity == "HIGH")] | length' "$OUTPUT_DIR/trivy-fs-scan.json" 2>/dev/null || echo "0")
        medium=$(jq '[.Results[]?.Vulnerabilities[]? | select(.Severity == "MEDIUM")] | length' "$OUTPUT_DIR/trivy-fs-scan.json" 2>/dev/null || echo "0")
        
        echo ""
        echo "Vulnerability Summary:"
        echo "  CRITICAL: $critical"
        echo "  HIGH: $high"
        echo "  MEDIUM: $medium"
        
        if [ "$critical" -gt 0 ] || [ "$high" -gt 0 ]; then
            echo ""
            echo -e "${RED}⚠ WARNING: Critical or High severity vulnerabilities found!${NC}"
            echo "Review reports in: $OUTPUT_DIR"
        fi
    fi
fi

# List generated files
echo ""
echo -e "${GREEN}Scan Complete${NC}"
echo "============="
echo "Output directory: $OUTPUT_DIR"
echo ""
echo "Generated reports:"
ls -lh "$OUTPUT_DIR"/*.{json,txt} 2>/dev/null | awk '{print "  " $9 " (" $5 ")"}' || echo "  No reports generated"

echo ""
echo -e "${GREEN}Next Steps:${NC}"
echo "1. Review vulnerability reports"
echo "2. Address CRITICAL and HIGH severity issues"
echo "3. Integrate into CI/CD pipeline"
echo "4. Set up automated alerts for new vulnerabilities"

