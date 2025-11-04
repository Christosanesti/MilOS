#!/bin/bash
# Qt6 Local Installation Script (User-space, no sudo required)
# This script downloads and sets up Qt6 in a local directory

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
QT6_DIR="$SCRIPT_DIR/../../.local/qt6"
QT6_VERSION="6.8.0"
QT6_ARCH="linux_x64"

echo "Qt6 Local Installation Script"
echo "=============================="
echo "Target directory: $QT6_DIR"
echo ""

# Create directory
mkdir -p "$QT6_DIR"
cd "$QT6_DIR"

echo "Step 1: Downloading Qt6 Online Installer..."
echo "Note: Qt requires an account. The installer will be downloaded to: $QT6_DIR"
echo ""
echo "Please download the Qt6 installer manually from:"
echo "https://www.qt.io/download-qt-installer"
echo ""
echo "Or use wget to download (requires Qt account):"
echo "wget https://download.qt.io/official_releases/online_installers/qt-unified-linux-x64-online.run"
echo ""
echo "After downloading, run the installer with:"
echo "chmod +x qt-unified-linux-x64-online.run"
echo "./qt-unified-linux-x64-online.run"
echo ""
echo "When installing, choose installation path: $QT6_DIR/Qt/$QT6_VERSION"
echo ""
echo "Alternative: Use system package manager (requires sudo but is safer):"
echo "sudo apt-get install -y qt6-base-dev qt6-base-dev-tools"

