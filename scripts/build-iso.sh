#!/bin/bash
# MilOS ISO Build Script
# Builds production-ready ISO image for MilOS distribution

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build-iso"
ISO_DIR="$BUILD_DIR/iso"
WORK_DIR="$BUILD_DIR/work"
ISO_NAME="milos-$(date +%Y%m%d)-x86_64.iso"
ISO_OUTPUT="$BUILD_DIR/$ISO_NAME"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== MilOS ISO Build Script ===${NC}"
echo "Project Root: $PROJECT_ROOT"
echo "Build Directory: $BUILD_DIR"
echo "ISO Output: $ISO_OUTPUT"
echo ""

# Check prerequisites
echo -e "${YELLOW}Checking prerequisites...${NC}"

check_command() {
    if ! command -v "$1" &> /dev/null; then
        echo -e "${RED}Error: $1 is not installed${NC}"
        exit 1
    fi
}

check_command mkisofs
check_command grub-mkrescue
check_command xorriso
check_command pacman  # For Arch-based builds
check_command gpg     # For package signing

echo -e "${GREEN}✓ All prerequisites met${NC}"
echo ""

# Create build directories
echo -e "${YELLOW}Creating build directories...${NC}"
mkdir -p "$ISO_DIR"
mkdir -p "$WORK_DIR"
mkdir -p "$ISO_DIR/boot/grub"
mkdir -p "$ISO_DIR/arch/x86_64"  # Package repository
echo -e "${GREEN}✓ Directories created${NC}"
echo ""

# Build all packages
echo -e "${YELLOW}Building MilOS packages...${NC}"
cd "$PROJECT_ROOT"

# Build base libraries
if [ -d "milos-base/libs/logging" ]; then
    echo "Building logging library..."
    cd milos-base/libs/logging
    mkdir -p build && cd build
    cmake .. && make -j$(nproc)
    cd "$PROJECT_ROOT"
fi

if [ -d "milos-base/libs/ui-utilities" ]; then
    echo "Building UI utilities library..."
    cd milos-base/libs/ui-utilities
    mkdir -p build && cd build
    cmake .. && make -j$(nproc)
    cd "$PROJECT_ROOT"
fi

# Build services
for service_dir in milos-base/services/*/; do
    if [ -f "$service_dir/CMakeLists.txt" ]; then
        service_name=$(basename "$service_dir")
        echo "Building service: $service_name"
        cd "$service_dir"
        mkdir -p build && cd build
        cmake .. && make -j$(nproc)
        cd "$PROJECT_ROOT"
    fi
done

# Build GUI tools
for tool_dir in milos-gui-tools/*/; do
    if [ -f "$tool_dir/CMakeLists.txt" ]; then
        tool_name=$(basename "$tool_dir")
        echo "Building GUI tool: $tool_name"
        cd "$tool_dir"
        mkdir -p build && cd build
        cmake .. && make -j$(nproc)
        cd "$PROJECT_ROOT"
    fi
done

# Build installer
if [ -d "milos-installer" ]; then
    echo "Building installer..."
    cd milos-installer
    mkdir -p build && cd build
    cmake .. && make -j$(nproc)
    cd "$PROJECT_ROOT"
fi

echo -e "${GREEN}✓ All packages built${NC}"
echo ""

# Install packages to ISO directory
echo -e "${YELLOW}Installing packages to ISO...${NC}"

# Install system files
INSTALL_ROOT="$ISO_DIR"
mkdir -p "$INSTALL_ROOT/usr/bin"
mkdir -p "$INSTALL_ROOT/usr/lib"
mkdir -p "$INSTALL_ROOT/usr/share/milos"
mkdir -p "$INSTALL_ROOT/etc/systemd/system"
mkdir -p "$INSTALL_ROOT/etc/dbus-1/system.d"

# Copy binaries
find "$PROJECT_ROOT" -name "*.so" -type f -exec cp {} "$INSTALL_ROOT/usr/lib/" \;
find "$PROJECT_ROOT" -type f -executable -path "*/build/*" -name "milos-*" -exec cp {} "$INSTALL_ROOT/usr/bin/" \;

# Copy systemd service files (if they exist)
if [ -d "$PROJECT_ROOT/milos-base/services" ]; then
    find "$PROJECT_ROOT/milos-base/services" -name "*.service" -exec cp {} "$INSTALL_ROOT/etc/systemd/system/" \;
fi

# Copy D-Bus configuration
if [ -d "$PROJECT_ROOT/milos-base/services" ]; then
    find "$PROJECT_ROOT/milos-base/services" -name "*.conf" -exec cp {} "$INSTALL_ROOT/etc/dbus-1/system.d/" \;
fi

# Copy installer
if [ -f "$PROJECT_ROOT/milos-installer/build/milos-installer" ]; then
    cp "$PROJECT_ROOT/milos-installer/build/milos-installer" "$INSTALL_ROOT/usr/bin/"
fi

echo -e "${GREEN}✓ Packages installed${NC}"
echo ""

# Create GRUB configuration
echo -e "${YELLOW}Creating GRUB configuration...${NC}"
cat > "$ISO_DIR/boot/grub/grub.cfg" << 'EOF'
set timeout=10
set default=0

menuentry "MilOS - Install" {
    linux /boot/vmlinuz-linux quiet
    initrd /boot/initramfs-linux.img
}

menuentry "MilOS - Live Mode" {
    linux /boot/vmlinuz-linux quiet systemd.unit=graphical.target
    initrd /boot/initramfs-linux.img
}
EOF

echo -e "${GREEN}✓ GRUB configuration created${NC}"
echo ""

# Create ISO
echo -e "${YELLOW}Creating ISO image...${NC}"
cd "$BUILD_DIR"

grub-mkrescue \
    --output="$ISO_OUTPUT" \
    --compression=xz \
    "$ISO_DIR" \
    2>&1 | tee "$BUILD_DIR/iso-build.log"

if [ $? -eq 0 ]; then
    echo ""
    echo -e "${GREEN}=== ISO Build Successful ===${NC}"
    echo "ISO Location: $ISO_OUTPUT"
    echo "ISO Size: $(du -h "$ISO_OUTPUT" | cut -f1)"
    echo ""
    echo "To test the ISO:"
    echo "  qemu-system-x86_64 -cdrom $ISO_OUTPUT -m 2048"
    echo ""
else
    echo -e "${RED}=== ISO Build Failed ===${NC}"
    echo "Check build log: $BUILD_DIR/iso-build.log"
    exit 1
fi



# MilOS ISO Build Script
# Builds production-ready ISO image for MilOS distribution

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build-iso"
ISO_DIR="$BUILD_DIR/iso"
WORK_DIR="$BUILD_DIR/work"
ISO_NAME="milos-$(date +%Y%m%d)-x86_64.iso"
ISO_OUTPUT="$BUILD_DIR/$ISO_NAME"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== MilOS ISO Build Script ===${NC}"
echo "Project Root: $PROJECT_ROOT"
echo "Build Directory: $BUILD_DIR"
echo "ISO Output: $ISO_OUTPUT"
echo ""

# Check prerequisites
echo -e "${YELLOW}Checking prerequisites...${NC}"

check_command() {
    if ! command -v "$1" &> /dev/null; then
        echo -e "${RED}Error: $1 is not installed${NC}"
        exit 1
    fi
}

check_command mkisofs
check_command grub-mkrescue
check_command xorriso
check_command pacman  # For Arch-based builds
check_command gpg     # For package signing

echo -e "${GREEN}✓ All prerequisites met${NC}"
echo ""

# Create build directories
echo -e "${YELLOW}Creating build directories...${NC}"
mkdir -p "$ISO_DIR"
mkdir -p "$WORK_DIR"
mkdir -p "$ISO_DIR/boot/grub"
mkdir -p "$ISO_DIR/arch/x86_64"  # Package repository
echo -e "${GREEN}✓ Directories created${NC}"
echo ""

# Build all packages
echo -e "${YELLOW}Building MilOS packages...${NC}"
cd "$PROJECT_ROOT"

# Build base libraries
if [ -d "milos-base/libs/logging" ]; then
    echo "Building logging library..."
    cd milos-base/libs/logging
    mkdir -p build && cd build
    cmake .. && make -j$(nproc)
    cd "$PROJECT_ROOT"
fi

if [ -d "milos-base/libs/ui-utilities" ]; then
    echo "Building UI utilities library..."
    cd milos-base/libs/ui-utilities
    mkdir -p build && cd build
    cmake .. && make -j$(nproc)
    cd "$PROJECT_ROOT"
fi

# Build services
for service_dir in milos-base/services/*/; do
    if [ -f "$service_dir/CMakeLists.txt" ]; then
        service_name=$(basename "$service_dir")
        echo "Building service: $service_name"
        cd "$service_dir"
        mkdir -p build && cd build
        cmake .. && make -j$(nproc)
        cd "$PROJECT_ROOT"
    fi
done

# Build GUI tools
for tool_dir in milos-gui-tools/*/; do
    if [ -f "$tool_dir/CMakeLists.txt" ]; then
        tool_name=$(basename "$tool_dir")
        echo "Building GUI tool: $tool_name"
        cd "$tool_dir"
        mkdir -p build && cd build
        cmake .. && make -j$(nproc)
        cd "$PROJECT_ROOT"
    fi
done

# Build installer
if [ -d "milos-installer" ]; then
    echo "Building installer..."
    cd milos-installer
    mkdir -p build && cd build
    cmake .. && make -j$(nproc)
    cd "$PROJECT_ROOT"
fi

echo -e "${GREEN}✓ All packages built${NC}"
echo ""

# Install packages to ISO directory
echo -e "${YELLOW}Installing packages to ISO...${NC}"

# Install system files
INSTALL_ROOT="$ISO_DIR"
mkdir -p "$INSTALL_ROOT/usr/bin"
mkdir -p "$INSTALL_ROOT/usr/lib"
mkdir -p "$INSTALL_ROOT/usr/share/milos"
mkdir -p "$INSTALL_ROOT/etc/systemd/system"
mkdir -p "$INSTALL_ROOT/etc/dbus-1/system.d"

# Copy binaries
find "$PROJECT_ROOT" -name "*.so" -type f -exec cp {} "$INSTALL_ROOT/usr/lib/" \;
find "$PROJECT_ROOT" -type f -executable -path "*/build/*" -name "milos-*" -exec cp {} "$INSTALL_ROOT/usr/bin/" \;

# Copy systemd service files (if they exist)
if [ -d "$PROJECT_ROOT/milos-base/services" ]; then
    find "$PROJECT_ROOT/milos-base/services" -name "*.service" -exec cp {} "$INSTALL_ROOT/etc/systemd/system/" \;
fi

# Copy D-Bus configuration
if [ -d "$PROJECT_ROOT/milos-base/services" ]; then
    find "$PROJECT_ROOT/milos-base/services" -name "*.conf" -exec cp {} "$INSTALL_ROOT/etc/dbus-1/system.d/" \;
fi

# Copy installer
if [ -f "$PROJECT_ROOT/milos-installer/build/milos-installer" ]; then
    cp "$PROJECT_ROOT/milos-installer/build/milos-installer" "$INSTALL_ROOT/usr/bin/"
fi

echo -e "${GREEN}✓ Packages installed${NC}"
echo ""

# Create GRUB configuration
echo -e "${YELLOW}Creating GRUB configuration...${NC}"
cat > "$ISO_DIR/boot/grub/grub.cfg" << 'EOF'
set timeout=10
set default=0

menuentry "MilOS - Install" {
    linux /boot/vmlinuz-linux quiet
    initrd /boot/initramfs-linux.img
}

menuentry "MilOS - Live Mode" {
    linux /boot/vmlinuz-linux quiet systemd.unit=graphical.target
    initrd /boot/initramfs-linux.img
}
EOF

echo -e "${GREEN}✓ GRUB configuration created${NC}"
echo ""

# Create ISO
echo -e "${YELLOW}Creating ISO image...${NC}"
cd "$BUILD_DIR"

grub-mkrescue \
    --output="$ISO_OUTPUT" \
    --compression=xz \
    "$ISO_DIR" \
    2>&1 | tee "$BUILD_DIR/iso-build.log"

if [ $? -eq 0 ]; then
    echo ""
    echo -e "${GREEN}=== ISO Build Successful ===${NC}"
    echo "ISO Location: $ISO_OUTPUT"
    echo "ISO Size: $(du -h "$ISO_OUTPUT" | cut -f1)"
    echo ""
    echo "To test the ISO:"
    echo "  qemu-system-x86_64 -cdrom $ISO_OUTPUT -m 2048"
    echo ""
else
    echo -e "${RED}=== ISO Build Failed ===${NC}"
    echo "Check build log: $BUILD_DIR/iso-build.log"
    exit 1
fi


