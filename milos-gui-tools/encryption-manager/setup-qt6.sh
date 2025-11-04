#!/bin/bash
# Qt6 Setup Script - Safe installation in user directory
# This script sets up Qt6 without requiring sudo and without affecting system packages

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
QT6_LOCAL_DIR="$PROJECT_ROOT/.local/qt6"
QT6_INSTALL_DIR="$QT6_LOCAL_DIR/Qt/6.8.0/gcc_64"

echo "=========================================="
echo "Qt6 Safe Installation Setup"
echo "=========================================="
echo ""
echo "This script will set up Qt6 in a local directory:"
echo "  $QT6_LOCAL_DIR"
echo ""
echo "This installation will:"
echo "  ✓ Not modify system packages"
echo "  ✓ Not require sudo privileges"
echo "  ✓ Be isolated to this project"
echo "  ✓ Not affect your Linux system stability"
echo ""

# Create directories
mkdir -p "$QT6_LOCAL_DIR"
cd "$QT6_LOCAL_DIR"

echo "Step 1: Checking for Qt6 installer..."
INSTALLER="qt-unified-linux-x64-online.run"
if [ ! -f "$INSTALLER" ]; then
    echo "Installer not found. Downloading Qt6 Online Installer..."
    echo ""
    echo "Please download the Qt6 installer from:"
    echo "  https://www.qt.io/download-qt-installer"
    echo ""
    echo "Or use this command:"
    echo "  wget https://download.qt.io/official_releases/online_installers/qt-unified-linux-x64-online.run"
    echo ""
    echo "Save it to: $QT6_LOCAL_DIR/$INSTALLER"
    echo ""
    read -p "Press Enter after downloading the installer, or Ctrl+C to cancel..."
fi

if [ -f "$INSTALLER" ]; then
    echo "Installer found: $INSTALLER"
    chmod +x "$INSTALLER"
    echo ""
    echo "Step 2: Running Qt6 installer..."
    echo ""
    echo "IMPORTANT: When the installer opens:"
    echo "  1. Sign in or create a Qt account"
    echo "  2. Select 'Qt 6.8.0' (or latest 6.x)"
    echo "  3. Set installation path to: $QT6_INSTALL_DIR"
    echo "  4. Select 'Desktop gcc 64-bit' component"
    echo "  5. Complete the installation"
    echo ""
    read -p "Press Enter to launch the installer, or Ctrl+C to cancel..."
    
    ./"$INSTALLER"
    
    if [ -d "$QT6_INSTALL_DIR" ]; then
        echo ""
        echo "✓ Qt6 installed successfully!"
        echo ""
        echo "Step 3: Configuring build system..."
        echo "  Qt6 location: $QT6_INSTALL_DIR"
        echo ""
        echo "You can now build the project with:"
        echo "  cd $SCRIPT_DIR"
        echo "  mkdir -p build && cd build"
        echo "  cmake .."
        echo "  make"
    else
        echo ""
        echo "⚠ Qt6 installation directory not found."
        echo "  Expected: $QT6_INSTALL_DIR"
        echo "  Please verify the installation completed successfully."
    fi
else
    echo ""
    echo "⚠ Installer not found. Please download it first."
    exit 1
fi

