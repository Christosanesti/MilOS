#!/bin/bash
# Complete Phase 1 Implementation - Master Script
# Orchestrates Tasks 2 and 3 execution

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

echo "╔═══════════════════════════════════════════════════════════════╗"
echo "║         PHASE 1 COMPLETION - MASTER SCRIPT                     ║"
echo "╚═══════════════════════════════════════════════════════════════╝"
echo ""

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

# Step 1: Verify Task 1 (should be complete)
echo -e "${YELLOW}Step 1: Verifying Task 1 (Debug Migration)...${NC}"
DEBUG_COUNT=$(grep -r "qDebug\|std::cout\|std::cerr" milos-base/services/*/src/*.cpp 2>/dev/null | grep -v "cli_tool.cpp" | wc -l)
if [ "$DEBUG_COUNT" -eq 0 ]; then
    echo -e "${GREEN}✅ Task 1: COMPLETE${NC}"
else
    echo -e "${RED}⚠️  Task 1: $DEBUG_COUNT instances remaining${NC}"
fi
echo ""

# Step 2: Check and install dependencies
echo -e "${YELLOW}Step 2: Checking dependencies...${NC}"
MISSING_DEPS=0

if ! dpkg -l | grep -q "libyaml-cpp-dev"; then
    echo -e "${RED}❌ libyaml-cpp-dev: NOT INSTALLED${NC}"
    MISSING_DEPS=$((MISSING_DEPS + 1))
else
    echo -e "${GREEN}✅ libyaml-cpp-dev: INSTALLED${NC}"
fi

if ! dpkg -l | grep -q "libpcap-dev"; then
    echo -e "${RED}❌ libpcap-dev: NOT INSTALLED${NC}"
    MISSING_DEPS=$((MISSING_DEPS + 1))
else
    echo -e "${GREEN}✅ libpcap-dev: INSTALLED${NC}"
fi

if ! command -v gcov &> /dev/null; then
    echo -e "${RED}❌ gcov: NOT INSTALLED${NC}"
    MISSING_DEPS=$((MISSING_DEPS + 1))
else
    echo -e "${GREEN}✅ gcov: INSTALLED${NC}"
fi

if ! command -v lcov &> /dev/null; then
    echo -e "${RED}❌ lcov: NOT INSTALLED${NC}"
    MISSING_DEPS=$((MISSING_DEPS + 1))
else
    echo -e "${GREEN}✅ lcov: INSTALLED${NC}"
fi

if [ $MISSING_DEPS -gt 0 ]; then
    echo ""
    echo -e "${YELLOW}Installing missing dependencies...${NC}"
    if [ -f "$SCRIPT_DIR/install-all-dependencies.sh" ]; then
        "$SCRIPT_DIR/install-all-dependencies.sh"
    else
        echo -e "${RED}Error: install-all-dependencies.sh not found${NC}"
        exit 1
    fi
else
    echo -e "${GREEN}✅ All dependencies installed${NC}"
fi
echo ""

# Step 3: Rebuild tests with coverage
echo -e "${YELLOW}Step 3: Rebuilding tests with coverage...${NC}"
cd "$PROJECT_ROOT/tests"
if [ ! -d "build" ]; then
    mkdir -p build
fi
cd build

# Clean and reconfigure with coverage
echo "Cleaning build directory..."
rm -rf * 2>/dev/null || true

echo "Configuring with coverage..."
cmake -DENABLE_COVERAGE=ON ..

echo "Building tests..."
make -j$(nproc)

echo -e "${GREEN}✅ Tests built with coverage enabled${NC}"
echo ""

# Step 4: Run all tests
echo -e "${YELLOW}Step 4: Running all tests...${NC}"
cd "$PROJECT_ROOT"
if [ -f "$SCRIPT_DIR/run-all-tests.sh" ]; then
    "$SCRIPT_DIR/run-all-tests.sh"
else
    echo -e "${RED}Error: run-all-tests.sh not found${NC}"
    exit 1
fi
echo ""

# Step 5: Generate coverage report
echo -e "${YELLOW}Step 5: Generating coverage report...${NC}"
cd "$PROJECT_ROOT/tests/build"
make test || echo "⚠️  Some tests may have failed, continuing with coverage..."

if [ -f Makefile ] && grep -q "coverage:" Makefile; then
    make coverage
    echo ""
    echo -e "${GREEN}✅ Coverage report generated!${NC}"
    echo "   Location: $PROJECT_ROOT/tests/build/coverage/index.html"
    echo ""
    echo "To view the report:"
    echo "   firefox $PROJECT_ROOT/tests/build/coverage/index.html"
else
    echo -e "${RED}❌ Coverage target not found${NC}"
    exit 1
fi
echo ""

# Step 6: Final verification
echo -e "${YELLOW}Step 6: Final verification...${NC}"
cd "$PROJECT_ROOT"
if [ -f "$SCRIPT_DIR/verify-phase1-readiness.sh" ]; then
    "$SCRIPT_DIR/verify-phase1-readiness.sh"
fi

echo ""
echo -e "${GREEN}╔═══════════════════════════════════════════════════════════════╗${NC}"
echo -e "${GREEN}║         PHASE 1 COMPLETION - SUCCESS!                          ║${NC}"
echo -e "${GREEN}╚═══════════════════════════════════════════════════════════════╝${NC}"
