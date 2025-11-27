#!/bin/bash
# MilOS ISO Build Test Script
# Tests the ISO build process and validates prerequisites

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== MilOS ISO Build Test ===${NC}"
echo ""

# Test 1: Validate build script syntax
echo -e "${YELLOW}Test 1: Validating build script syntax...${NC}"
if bash -n "$SCRIPT_DIR/build-iso.sh" 2>&1; then
    echo -e "${GREEN}✓ Build script syntax is valid${NC}"
else
    echo -e "${RED}✗ Build script has syntax errors${NC}"
    exit 1
fi
echo ""

# Test 2: Check prerequisites
echo -e "${YELLOW}Test 2: Checking prerequisites...${NC}"
MISSING_PREREQS=0

check_prereq() {
    if command -v "$1" &> /dev/null; then
        echo -e "${GREEN}✓ $1 found${NC}"
        return 0
    else
        echo -e "${RED}✗ $1 not found${NC}"
        MISSING_PREREQS=$((MISSING_PREREQS + 1))
        return 1
    fi
}

check_prereq mkisofs || echo "  Install: genisoimage package"
check_prereq grub-mkrescue || echo "  Install: grub package"
check_prereq xorriso || echo "  Install: libisoburn package"
check_prereq pacman || echo "  Install: pacman package"
check_prereq gpg || echo "  Install: gnupg package"
check_prereq cmake || echo "  Install: cmake package"
check_prereq make || echo "  Install: make package"

if [ $MISSING_PREREQS -gt 0 ]; then
    echo ""
    echo -e "${RED}✗ Missing $MISSING_PREREQS prerequisites${NC}"
    echo "Please install missing packages before building ISO"
    exit 1
fi
echo ""

# Test 3: Check project structure
echo -e "${YELLOW}Test 3: Validating project structure...${NC}"
REQUIRED_DIRS=(
    "milos-base/libs/logging"
    "milos-base/libs/ui-utilities"
    "milos-base/services"
    "milos-gui-tools"
    "milos-installer"
)

MISSING_DIRS=0
for dir in "${REQUIRED_DIRS[@]}"; do
    if [ -d "$PROJECT_ROOT/$dir" ]; then
        echo -e "${GREEN}✓ $dir exists${NC}"
    else
        echo -e "${RED}✗ $dir missing${NC}"
        MISSING_DIRS=$((MISSING_DIRS + 1))
    fi
done

if [ $MISSING_DIRS -gt 0 ]; then
    echo ""
    echo -e "${RED}✗ Missing $MISSING_DIRS required directories${NC}"
    exit 1
fi
echo ""

# Test 4: Check CMakeLists.txt files
echo -e "${YELLOW}Test 4: Checking CMakeLists.txt files...${NC}"
MISSING_CMAKE=0

if [ -f "$PROJECT_ROOT/milos-base/libs/logging/CMakeLists.txt" ]; then
    echo -e "${GREEN}✓ logging/CMakeLists.txt exists${NC}"
else
    echo -e "${RED}✗ logging/CMakeLists.txt missing${NC}"
    MISSING_CMAKE=$((MISSING_CMAKE + 1))
fi

if [ -f "$PROJECT_ROOT/milos-base/libs/ui-utilities/CMakeLists.txt" ]; then
    echo -e "${GREEN}✓ ui-utilities/CMakeLists.txt exists${NC}"
else
    echo -e "${RED}✗ ui-utilities/CMakeLists.txt missing${NC}"
    MISSING_CMAKE=$((MISSING_CMAKE + 1))
fi

if [ $MISSING_CMAKE -gt 0 ]; then
    echo ""
    echo -e "${RED}✗ Missing $MISSING_CMAKE CMakeLists.txt files${NC}"
    exit 1
fi
echo ""

# Test 5: Check disk space (need at least 5GB free)
echo -e "${YELLOW}Test 5: Checking disk space...${NC}"
AVAILABLE_SPACE=$(df -BG "$PROJECT_ROOT" | tail -1 | awk '{print $4}' | sed 's/G//')
if [ "$AVAILABLE_SPACE" -ge 5 ]; then
    echo -e "${GREEN}✓ Sufficient disk space: ${AVAILABLE_SPACE}GB available${NC}"
else
    echo -e "${RED}✗ Insufficient disk space: ${AVAILABLE_SPACE}GB available (need at least 5GB)${NC}"
    exit 1
fi
echo ""

# Test 6: Check write permissions
echo -e "${YELLOW}Test 6: Checking write permissions...${NC}"
if [ -w "$PROJECT_ROOT" ]; then
    echo -e "${GREEN}✓ Write permissions OK${NC}"
else
    echo -e "${RED}✗ No write permissions in project directory${NC}"
    exit 1
fi
echo ""

# Summary
echo -e "${GREEN}=== All Tests Passed ===${NC}"
echo ""
echo "ISO build prerequisites validated successfully."
echo "You can now run: ./scripts/build-iso.sh"
echo ""
echo "Note: This is a dry-run test. Actual ISO build will:"
echo "  - Build all packages (may take 30+ minutes)"
echo "  - Create ISO image (may take 10+ minutes)"
echo "  - Require significant disk space"



# MilOS ISO Build Test Script
# Tests the ISO build process and validates prerequisites

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== MilOS ISO Build Test ===${NC}"
echo ""

# Test 1: Validate build script syntax
echo -e "${YELLOW}Test 1: Validating build script syntax...${NC}"
if bash -n "$SCRIPT_DIR/build-iso.sh" 2>&1; then
    echo -e "${GREEN}✓ Build script syntax is valid${NC}"
else
    echo -e "${RED}✗ Build script has syntax errors${NC}"
    exit 1
fi
echo ""

# Test 2: Check prerequisites
echo -e "${YELLOW}Test 2: Checking prerequisites...${NC}"
MISSING_PREREQS=0

check_prereq() {
    if command -v "$1" &> /dev/null; then
        echo -e "${GREEN}✓ $1 found${NC}"
        return 0
    else
        echo -e "${RED}✗ $1 not found${NC}"
        MISSING_PREREQS=$((MISSING_PREREQS + 1))
        return 1
    fi
}

check_prereq mkisofs || echo "  Install: genisoimage package"
check_prereq grub-mkrescue || echo "  Install: grub package"
check_prereq xorriso || echo "  Install: libisoburn package"
check_prereq pacman || echo "  Install: pacman package"
check_prereq gpg || echo "  Install: gnupg package"
check_prereq cmake || echo "  Install: cmake package"
check_prereq make || echo "  Install: make package"

if [ $MISSING_PREREQS -gt 0 ]; then
    echo ""
    echo -e "${RED}✗ Missing $MISSING_PREREQS prerequisites${NC}"
    echo "Please install missing packages before building ISO"
    exit 1
fi
echo ""

# Test 3: Check project structure
echo -e "${YELLOW}Test 3: Validating project structure...${NC}"
REQUIRED_DIRS=(
    "milos-base/libs/logging"
    "milos-base/libs/ui-utilities"
    "milos-base/services"
    "milos-gui-tools"
    "milos-installer"
)

MISSING_DIRS=0
for dir in "${REQUIRED_DIRS[@]}"; do
    if [ -d "$PROJECT_ROOT/$dir" ]; then
        echo -e "${GREEN}✓ $dir exists${NC}"
    else
        echo -e "${RED}✗ $dir missing${NC}"
        MISSING_DIRS=$((MISSING_DIRS + 1))
    fi
done

if [ $MISSING_DIRS -gt 0 ]; then
    echo ""
    echo -e "${RED}✗ Missing $MISSING_DIRS required directories${NC}"
    exit 1
fi
echo ""

# Test 4: Check CMakeLists.txt files
echo -e "${YELLOW}Test 4: Checking CMakeLists.txt files...${NC}"
MISSING_CMAKE=0

if [ -f "$PROJECT_ROOT/milos-base/libs/logging/CMakeLists.txt" ]; then
    echo -e "${GREEN}✓ logging/CMakeLists.txt exists${NC}"
else
    echo -e "${RED}✗ logging/CMakeLists.txt missing${NC}"
    MISSING_CMAKE=$((MISSING_CMAKE + 1))
fi

if [ -f "$PROJECT_ROOT/milos-base/libs/ui-utilities/CMakeLists.txt" ]; then
    echo -e "${GREEN}✓ ui-utilities/CMakeLists.txt exists${NC}"
else
    echo -e "${RED}✗ ui-utilities/CMakeLists.txt missing${NC}"
    MISSING_CMAKE=$((MISSING_CMAKE + 1))
fi

if [ $MISSING_CMAKE -gt 0 ]; then
    echo ""
    echo -e "${RED}✗ Missing $MISSING_CMAKE CMakeLists.txt files${NC}"
    exit 1
fi
echo ""

# Test 5: Check disk space (need at least 5GB free)
echo -e "${YELLOW}Test 5: Checking disk space...${NC}"
AVAILABLE_SPACE=$(df -BG "$PROJECT_ROOT" | tail -1 | awk '{print $4}' | sed 's/G//')
if [ "$AVAILABLE_SPACE" -ge 5 ]; then
    echo -e "${GREEN}✓ Sufficient disk space: ${AVAILABLE_SPACE}GB available${NC}"
else
    echo -e "${RED}✗ Insufficient disk space: ${AVAILABLE_SPACE}GB available (need at least 5GB)${NC}"
    exit 1
fi
echo ""

# Test 6: Check write permissions
echo -e "${YELLOW}Test 6: Checking write permissions...${NC}"
if [ -w "$PROJECT_ROOT" ]; then
    echo -e "${GREEN}✓ Write permissions OK${NC}"
else
    echo -e "${RED}✗ No write permissions in project directory${NC}"
    exit 1
fi
echo ""

# Summary
echo -e "${GREEN}=== All Tests Passed ===${NC}"
echo ""
echo "ISO build prerequisites validated successfully."
echo "You can now run: ./scripts/build-iso.sh"
echo ""
echo "Note: This is a dry-run test. Actual ISO build will:"
echo "  - Build all packages (may take 30+ minutes)"
echo "  - Create ISO image (may take 10+ minutes)"
echo "  - Require significant disk space"


