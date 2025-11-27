#!/bin/bash
# MilOS Complete PKGBUILD Generator
# Generates all PKGBUILD files and sets up repository structure

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
PKGBUILD_DIR="$PROJECT_ROOT/pkgbuilds"
REPO_DIR="$PROJECT_ROOT/repo/x86_64"

echo "=== MilOS Complete PKGBUILD Generation ==="
echo "PKGBUILD Directory: $PKGBUILD_DIR"
echo "Repository Directory: $REPO_DIR"
echo ""

# Create directories
mkdir -p "$PKGBUILD_DIR"
mkdir -p "$REPO_DIR"

# Run PKGBUILD generator
echo "Generating PKGBUILD files..."
bash "$SCRIPT_DIR/create-pkgbuild.sh"

echo ""
echo "=== PKGBUILD Files Generated ==="
echo "Location: $PKGBUILD_DIR"
echo ""
echo "Next steps:"
echo "  1. Review PKGBUILD files"
echo "  2. Build packages: cd $PKGBUILD_DIR && makepkg -s"
echo "  3. Add to repository: bash $SCRIPT_DIR/setup-repository.sh"
echo ""



# MilOS Complete PKGBUILD Generator
# Generates all PKGBUILD files and sets up repository structure

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
PKGBUILD_DIR="$PROJECT_ROOT/pkgbuilds"
REPO_DIR="$PROJECT_ROOT/repo/x86_64"

echo "=== MilOS Complete PKGBUILD Generation ==="
echo "PKGBUILD Directory: $PKGBUILD_DIR"
echo "Repository Directory: $REPO_DIR"
echo ""

# Create directories
mkdir -p "$PKGBUILD_DIR"
mkdir -p "$REPO_DIR"

# Run PKGBUILD generator
echo "Generating PKGBUILD files..."
bash "$SCRIPT_DIR/create-pkgbuild.sh"

echo ""
echo "=== PKGBUILD Files Generated ==="
echo "Location: $PKGBUILD_DIR"
echo ""
echo "Next steps:"
echo "  1. Review PKGBUILD files"
echo "  2. Build packages: cd $PKGBUILD_DIR && makepkg -s"
echo "  3. Add to repository: bash $SCRIPT_DIR/setup-repository.sh"
echo ""


