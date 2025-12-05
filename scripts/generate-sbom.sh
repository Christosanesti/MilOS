#!/bin/bash
#
# SBOM Generation Script for MilOS
# Generates Software Bill of Materials (SBOM) in CycloneDX format
# Required for NIST SP 800-161 compliance
#
# Usage: ./generate-sbom.sh [output-dir]
#

set -e

OUTPUT_DIR="${1:-./sbom}"
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SBOM_TOOL="cyclonedx"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}MilOS SBOM Generation${NC}"
echo "========================"
echo ""

# Check if cyclonedx is installed
if ! command -v "$SBOM_TOOL" &> /dev/null; then
    echo -e "${YELLOW}Warning: cyclonedx not found. Attempting to download...${NC}"
    
    # Try to download cyclonedx
    CYCLONEDX_VERSION="0.24.2"
    CYCLONEDX_URL="https://github.com/CycloneDX/cyclonedx-cli/releases/download/v${CYCLONEDX_VERSION}/cyclonedx-linux-amd64"
    
    if command -v wget &> /dev/null; then
        echo "Downloading cyclonedx..."
        wget -q "$CYCLONEDX_URL" -O /tmp/cyclonedx 2>&1
        if [ $? -eq 0 ] && [ -f /tmp/cyclonedx ]; then
            chmod +x /tmp/cyclonedx
            SBOM_TOOL="/tmp/cyclonedx"
            echo "cyclonedx downloaded successfully"
        else
            echo -e "${RED}Error: Failed to download cyclonedx${NC}"
            exit 1
        fi
    elif command -v curl &> /dev/null; then
        echo "Downloading cyclonedx..."
        curl -L "$CYCLONEDX_URL" -o /tmp/cyclonedx 2>&1
        if [ $? -eq 0 ] && [ -f /tmp/cyclonedx ]; then
            chmod +x /tmp/cyclonedx
            SBOM_TOOL="/tmp/cyclonedx"
            echo "cyclonedx downloaded successfully"
        else
            echo -e "${RED}Error: Failed to download cyclonedx${NC}"
            exit 1
        fi
    else
        echo -e "${RED}Error: cyclonedx not found and cannot download (wget/curl not available)${NC}"
        echo "Please install cyclonedx manually:"
        echo "  wget https://github.com/CycloneDX/cyclonedx-cli/releases/latest/download/cyclonedx-linux-amd64"
        echo "  chmod +x cyclonedx-linux-amd64"
        echo "  sudo mv cyclonedx-linux-amd64 /usr/local/bin/cyclonedx"
        exit 1
    fi
fi

# Create output directory
mkdir -p "$OUTPUT_DIR"

echo "Project Root: $PROJECT_ROOT"
echo "Output Directory: $OUTPUT_DIR"
echo "SBOM Tool: $SBOM_TOOL"
echo ""

# Generate SBOM for main project
echo -e "${GREEN}Generating SBOM for MilOS...${NC}"

# Try different methods based on what's available
if [ -f "$PROJECT_ROOT/CMakeLists.txt" ]; then
    echo "Detected CMake project"
    
    # Generate SBOM from CMake build directory if it exists
    if [ -d "$PROJECT_ROOT/build" ]; then
        echo "Scanning build directory..."
        "$SBOM_TOOL" bom \
            --input-dir "$PROJECT_ROOT/build" \
            --output-file "$OUTPUT_DIR/milos-sbom.json" \
            --output-format json \
            --output-version 1.5 || {
            echo -e "${YELLOW}Warning: Build directory scan failed, trying source directory...${NC}"
        }
    fi
fi

# Generate SBOM from source directory (fallback)
if [ ! -f "$OUTPUT_DIR/milos-sbom.json" ]; then
    echo "Scanning source directory..."
    "$SBOM_TOOL" bom \
        --input-dir "$PROJECT_ROOT" \
        --output-file "$OUTPUT_DIR/milos-sbom.json" \
        --output-format json \
        --output-version 1.5 \
        --exclude "**/build/**" \
        --exclude "**/.git/**" \
        --exclude "**/node_modules/**" \
        --exclude "**/__pycache__/**" || {
        echo -e "${RED}Error: Failed to generate SBOM${NC}"
        exit 1
    }
fi

# Generate SBOMs for individual services
echo ""
echo -e "${GREEN}Generating SBOMs for individual services...${NC}"

SERVICES_DIR="$PROJECT_ROOT/milos-base/services"
if [ -d "$SERVICES_DIR" ]; then
    for service_dir in "$SERVICES_DIR"/*; do
        if [ -d "$service_dir" ]; then
            service_name=$(basename "$service_dir")
            echo "  - $service_name"
            
            "$SBOM_TOOL" bom \
                --input-dir "$service_dir" \
                --output-file "$OUTPUT_DIR/${service_name}-sbom.json" \
                --output-format json \
                --output-version 1.5 \
                --exclude "**/build/**" \
                --exclude "**/.git/**" 2>/dev/null || {
                echo -e "    ${YELLOW}Warning: Failed to generate SBOM for $service_name${NC}"
            }
        fi
    done
fi

# Validate SBOM format
echo ""
echo -e "${GREEN}Validating SBOM format...${NC}"

if command -v jq &> /dev/null; then
    for sbom_file in "$OUTPUT_DIR"/*-sbom.json; do
        if [ -f "$sbom_file" ]; then
            filename=$(basename "$sbom_file")
            if jq empty "$sbom_file" 2>/dev/null; then
                echo -e "  ${GREEN}✓${NC} $filename (valid JSON)"
                
                # Check for required CycloneDX fields
                if jq -e '.bomFormat' "$sbom_file" > /dev/null 2>&1; then
                    bom_format=$(jq -r '.bomFormat' "$sbom_file")
                    spec_version=$(jq -r '.specVersion' "$sbom_file" 2>/dev/null || echo "unknown")
                    echo "    Format: $bom_format, Spec: $spec_version"
                fi
            else
                echo -e "  ${RED}✗${NC} $filename (invalid JSON)"
            fi
        fi
    done
else
    echo -e "${YELLOW}Warning: jq not installed, skipping validation${NC}"
fi

# Summary
echo ""
echo -e "${GREEN}SBOM Generation Complete${NC}"
echo "========================"
echo "Output directory: $OUTPUT_DIR"
echo ""

# List generated files
if [ -d "$OUTPUT_DIR" ]; then
    echo "Generated SBOM files:"
    ls -lh "$OUTPUT_DIR"/*.json 2>/dev/null | awk '{print "  " $9 " (" $5 ")"}' || echo "  No SBOM files generated"
fi

echo ""
echo -e "${GREEN}Next Steps:${NC}"
echo "1. Review generated SBOM files"
echo "2. Integrate into build pipeline"
echo "3. Store SBOM with build artifacts"
echo "4. Include SBOM in release packages"

