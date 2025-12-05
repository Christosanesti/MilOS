#!/bin/bash
# Install all dependencies for MilOS development and testing

set -e

echo "=== MilOS Dependency Installation Script ==="
echo ""

# Test dependencies
echo "Installing test dependencies..."
sudo apt-get update
sudo apt-get install -y \
    libyaml-cpp-dev \
    libpcap-dev

# ISO build dependencies (optional)
echo ""
read -p "Install ISO build dependencies? (y/n) " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    sudo apt-get install -y \
        genisoimage \
        libisoburn \
        grub-common
    echo "Note: pacman is Arch-specific and may not be available on Debian/Ubuntu"
fi

echo ""
echo "=== Dependencies installed successfully ==="
