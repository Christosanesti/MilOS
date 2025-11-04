#!/bin/bash
# Qt6 Installation Script for Encryption Manager
# Run this script with: bash install-qt6.sh

set -e

echo "Installing Qt6 development packages..."
sudo apt-get update
sudo apt-get install -y \
    qt6-base-dev \
    qt6-base-dev-tools \
    qt6-tools-dev \
    cmake \
    build-essential

echo ""
echo "Verifying installation..."
cmake --version
qmake6 --version 2>/dev/null || echo "Note: qmake6 may not be in PATH, but Qt6 should be installed"

echo ""
echo "Qt6 installation complete!"
echo "You can now build the project with:"
echo "  cd build && cmake .. && make"

