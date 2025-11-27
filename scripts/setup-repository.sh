#!/bin/bash
# MilOS Package Repository Setup Script
# Sets up pacman-compatible repository for MilOS packages

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
REPO_DIR="$PROJECT_ROOT/repo"
REPO_NAME="milos"

echo "=== MilOS Repository Setup ==="
echo "Repository Directory: $REPO_DIR"
echo ""

# Create repository directory
mkdir -p "$REPO_DIR/x86_64"

# Check if repo-add is available
if ! command -v repo-add &> /dev/null; then
    echo "Error: repo-add not found. Install pacman-contrib package."
    exit 1
fi

# Check for GPG key for signing
if [ -z "$GPG_KEY_ID" ]; then
    echo "Warning: GPG_KEY_ID not set. Packages will not be signed."
    echo "Set GPG_KEY_ID environment variable to enable signing."
    SIGN_OPTS=""
else
    SIGN_OPTS="--sign --key $GPG_KEY_ID"
    echo "Using GPG key: $GPG_KEY_ID"
fi

echo "Repository structure created."
echo ""
echo "To add packages to repository:"
echo "  1. Build packages using create-pkgbuild.sh"
echo "  2. Build packages: makepkg"
echo "  3. Add to repo: repo-add $SIGN_OPTS $REPO_DIR/x86_64/$REPO_NAME.db.tar.xz *.pkg.tar.xz"
echo ""
echo "To use repository, add to /etc/pacman.conf:"
echo "  [milos]"
echo "  SigLevel = Optional"
echo "  Server = file://$REPO_DIR"
echo ""



# MilOS Package Repository Setup Script
# Sets up pacman-compatible repository for MilOS packages

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
REPO_DIR="$PROJECT_ROOT/repo"
REPO_NAME="milos"

echo "=== MilOS Repository Setup ==="
echo "Repository Directory: $REPO_DIR"
echo ""

# Create repository directory
mkdir -p "$REPO_DIR/x86_64"

# Check if repo-add is available
if ! command -v repo-add &> /dev/null; then
    echo "Error: repo-add not found. Install pacman-contrib package."
    exit 1
fi

# Check for GPG key for signing
if [ -z "$GPG_KEY_ID" ]; then
    echo "Warning: GPG_KEY_ID not set. Packages will not be signed."
    echo "Set GPG_KEY_ID environment variable to enable signing."
    SIGN_OPTS=""
else
    SIGN_OPTS="--sign --key $GPG_KEY_ID"
    echo "Using GPG key: $GPG_KEY_ID"
fi

echo "Repository structure created."
echo ""
echo "To add packages to repository:"
echo "  1. Build packages using create-pkgbuild.sh"
echo "  2. Build packages: makepkg"
echo "  3. Add to repo: repo-add $SIGN_OPTS $REPO_DIR/x86_64/$REPO_NAME.db.tar.xz *.pkg.tar.xz"
echo ""
echo "To use repository, add to /etc/pacman.conf:"
echo "  [milos]"
echo "  SigLevel = Optional"
echo "  Server = file://$REPO_DIR"
echo ""


