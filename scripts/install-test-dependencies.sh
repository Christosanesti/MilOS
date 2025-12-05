#!/bin/bash
# Install test dependencies for MilOS test suite

set -e

echo "Installing test dependencies for MilOS..."
echo ""

# Update package list
sudo apt-get update

# Install yaml-cpp development package
echo "Installing libyaml-cpp-dev..."
sudo apt-get install -y libyaml-cpp-dev

# Install libpcap development package
echo "Installing libpcap-dev..."
sudo apt-get install -y libpcap-dev

# Verify installation
echo ""
echo "Verifying installation..."
pkg-config --exists yaml-cpp && echo "✅ yaml-cpp: FOUND" || echo "❌ yaml-cpp: NOT FOUND"
pkg-config --exists libpcap && echo "✅ libpcap: FOUND" || echo "❌ libpcap: NOT FOUND"

echo ""
echo "Dependencies installed successfully!"
echo "You can now rebuild the test suite:"
echo "  cd tests/build"
echo "  rm -rf *"
echo "  cmake -DENABLE_COVERAGE=ON .."
echo "  make -j\$(nproc)"

