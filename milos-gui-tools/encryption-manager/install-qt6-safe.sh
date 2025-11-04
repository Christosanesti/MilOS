#!/bin/bash
# Safe Qt6 Installation Script
# This installs Qt6 via Ubuntu's official package manager (apt)
# This is the SAFEST method - uses official Ubuntu packages that won't break your system

set -e

echo "=========================================="
echo "Qt6 Safe Installation via Ubuntu Packages"
echo "=========================================="
echo ""
echo "This script will install Qt6 development packages using Ubuntu's"
echo "official package manager (apt). This is the SAFEST and RECOMMENDED"
echo "method because:"
echo ""
echo "  ✓ Uses official Ubuntu repositories"
echo "  ✓ Packages are tested and stable"
echo "  ✓ No risk of breaking your system"
echo "  ✓ Proper dependency management"
echo "  ✓ Can be easily removed if needed"
echo ""
echo "Packages to be installed:"
echo "  - qt6-base-dev          (Qt6 core and widgets development files)"
echo "  - qt6-base-dev-tools    (Qt6 development tools: moc, rcc, uic)"
echo "  - qt6-tools-dev         (Qt6 additional development tools)"
echo "  - cmake                 (Build system - may already be installed)"
echo "  - build-essential       (C++ compiler and build tools)"
echo ""
read -p "Press Enter to continue with installation, or Ctrl+C to cancel..."
echo ""

echo "Updating package lists..."
sudo apt-get update

echo ""
echo "Installing Qt6 development packages..."
sudo apt-get install -y \
    qt6-base-dev \
    qt6-base-dev-tools \
    qt6-tools-dev \
    cmake \
    build-essential

echo ""
echo "=========================================="
echo "✓ Qt6 Installation Complete!"
echo "=========================================="
echo ""
echo "Verifying installation..."
echo ""

if command -v qmake6 &> /dev/null; then
    qmake6 --version
elif [ -f /usr/lib/x86_64-linux-gnu/qt6/bin/qmake ]; then
    /usr/lib/x86_64-linux-gnu/qt6/bin/qmake --version
else
    echo "Qt6 installed. Checking CMake configuration..."
fi

cmake --version

echo ""
echo "You can now build the project with:"
echo "  cd $(dirname "$0")"
echo "  mkdir -p build && cd build"
echo "  cmake .."
echo "  make"
echo ""

