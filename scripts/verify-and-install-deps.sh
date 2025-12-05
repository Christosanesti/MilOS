#!/bin/bash
# Verify dependencies and provide installation instructions

set -e

echo "=========================================="
echo "MilOS Dependency Checker"
echo "=========================================="
echo ""

# Check for yaml-cpp
if pkg-config --exists yaml-cpp 2>/dev/null; then
    YAML_VERSION=$(pkg-config --modversion yaml-cpp)
    echo "✅ yaml-cpp: FOUND (version $YAML_VERSION)"
    YAML_OK=true
else
    echo "❌ yaml-cpp: NOT FOUND"
    YAML_OK=false
fi

# Check for libpcap
if pkg-config --exists libpcap 2>/dev/null; then
    PCAP_VERSION=$(pkg-config --modversion libpcap)
    echo "✅ libpcap: FOUND (version $PCAP_VERSION)"
    PCAP_OK=true
else
    echo "❌ libpcap: NOT FOUND"
    PCAP_OK=false
fi

echo ""

if [ "$YAML_OK" = true ] && [ "$PCAP_OK" = true ]; then
    echo "✅ All dependencies are installed!"
    echo ""
    echo "You can now rebuild and run tests:"
    echo "  cd tests/build"
    echo "  rm -rf *"
    echo "  cmake -DENABLE_COVERAGE=ON .."
    echo "  make -j\$(nproc)"
    echo "  cd ../.."
    echo "  ./scripts/run-all-tests.sh"
    exit 0
else
    echo "⚠️  Missing dependencies detected"
    echo ""
    echo "To install missing dependencies, run:"
    echo "  sudo apt-get update"
    echo "  sudo apt-get install -y libyaml-cpp-dev libpcap-dev"
    echo ""
    echo "After installation, run this script again to verify."
    exit 1
fi

