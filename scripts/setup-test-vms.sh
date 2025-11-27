#!/bin/bash
# MilOS Test VM Setup Script
# Sets up test VMs for ISO installation testing

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
VM_DIR="$PROJECT_ROOT/test-vms"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== MilOS Test VM Setup ===${NC}"
echo "VM Directory: $VM_DIR"
echo ""

# Check prerequisites
check_command() {
    if ! command -v "$1" &> /dev/null; then
        echo -e "${YELLOW}Warning: $1 is not installed${NC}"
        return 1
    fi
    return 0
}

echo -e "${YELLOW}Checking prerequisites...${NC}"

HAS_QEMU=false
HAS_VBOX=false

if check_command qemu-system-x86_64; then
    HAS_QEMU=true
    echo -e "${GREEN}✓ QEMU/KVM available${NC}"
fi

if check_command VBoxManage; then
    HAS_VBOX=true
    echo -e "${GREEN}✓ VirtualBox available${NC}"
fi

if [ "$HAS_QEMU" = false ] && [ "$HAS_VBOX" = false ]; then
    echo -e "${RED}Error: No virtualization software found${NC}"
    echo "Please install QEMU/KVM or VirtualBox"
    exit 1
fi

echo ""

# Create VM directory
mkdir -p "$VM_DIR"

# QEMU/KVM VM setup
if [ "$HAS_QEMU" = true ]; then
    echo -e "${YELLOW}Setting up QEMU/KVM test VM...${NC}"
    
    QEMU_VM_DIR="$VM_DIR/qemu"
    mkdir -p "$QEMU_VM_DIR"
    
    # Create VM disk image (20GB)
    if [ ! -f "$QEMU_VM_DIR/milos-test-disk.qcow2" ]; then
        echo "Creating QEMU disk image (20GB)..."
        qemu-img create -f qcow2 "$QEMU_VM_DIR/milos-test-disk.qcow2" 20G
        echo -e "${GREEN}✓ QEMU disk image created${NC}"
    else
        echo -e "${YELLOW}QEMU disk image already exists${NC}"
    fi
    
    # Create VM launch script
    cat > "$QEMU_VM_DIR/launch-vm.sh" << 'EOF'
#!/bin/bash
# Launch MilOS test VM with QEMU

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ISO_PATH="${1:-../../build-iso/milos-*.iso}"

if [ ! -f "$ISO_PATH" ]; then
    echo "Error: ISO file not found: $ISO_PATH"
    echo "Usage: $0 [path-to-iso]"
    exit 1
fi

qemu-system-x86_64 \
    -cdrom "$ISO_PATH" \
    -hda "$SCRIPT_DIR/milos-test-disk.qcow2" \
    -m 2048 \
    -smp 2 \
    -boot d \
    -enable-kvm \
    -vga virtio \
    -display gtk
EOF
    
    chmod +x "$QEMU_VM_DIR/launch-vm.sh"
    echo -e "${GREEN}✓ QEMU VM setup complete${NC}"
    echo "  Launch with: $QEMU_VM_DIR/launch-vm.sh [path-to-iso]"
    echo ""
fi

# VirtualBox VM setup
if [ "$HAS_VBOX" = true ]; then
    echo -e "${YELLOW}Setting up VirtualBox test VM...${NC}"
    
    VM_NAME="MilOS-Test"
    
    # Check if VM already exists
    if VBoxManage list vms | grep -q "$VM_NAME"; then
        echo -e "${YELLOW}VirtualBox VM '$VM_NAME' already exists${NC}"
        echo "  To recreate, delete it first: VBoxManage unregistervm '$VM_NAME' --delete"
    else
        echo "Creating VirtualBox VM..."
        
        # Create VM
        VBoxManage createvm --name "$VM_NAME" --register --ostype "Linux_64"
        
        # Configure VM
        VBoxManage modifyvm "$VM_NAME" \
            --memory 2048 \
            --cpus 2 \
            --vram 128 \
            --boot1 dvd \
            --boot2 disk \
            --boot3 none \
            --boot4 none \
            --acpi on \
            --ioapic on \
            --pae on \
            --longmode on \
            --largepages on \
            --paravirtprovider kvm
        
        # Create disk
        VBOX_DISK="$VM_DIR/vbox/milos-test-disk.vdi"
        mkdir -p "$VM_DIR/vbox"
        VBoxManage createhd --filename "$VBOX_DISK" --size 20480 --format VDI
        
        # Attach disk
        VBoxManage storagectl "$VM_NAME" --name "SATA" --add sata --controller IntelAHCI
        VBoxManage storageattach "$VM_NAME" --storagectl "SATA" --port 0 --device 0 --type hdd --medium "$VBOX_DISK"
        
        # Attach DVD (for ISO)
        VBoxManage storagectl "$VM_NAME" --name "IDE" --add ide
        echo -e "${GREEN}✓ VirtualBox VM created${NC}"
        echo "  To attach ISO: VBoxManage storageattach '$VM_NAME' --storagectl IDE --port 0 --device 0 --type dvddrive --medium [path-to-iso]"
        echo "  To start VM: VBoxManage startvm '$VM_NAME'"
    fi
    echo ""
fi

# Create test checklist
cat > "$VM_DIR/TEST-CHECKLIST.md" << 'EOF'
# MilOS ISO Installation Test Checklist

## Pre-Installation
- [ ] ISO boots successfully
- [ ] GRUB menu appears
- [ ] Live mode works (if available)
- [ ] Installer launches

## Installation Process
- [ ] Welcome screen displays
- [ ] Partitioning works (auto and manual)
- [ ] Encryption setup works
- [ ] User account creation works
- [ ] Network configuration works
- [ ] Hardware detection works
- [ ] Package installation completes
- [ ] Installation completes successfully

## Post-Installation
- [ ] System boots from installed disk
- [ ] All services start correctly
- [ ] D-Bus interfaces work
- [ ] GUI applications launch
- [ ] CLI tools work
- [ ] Network connectivity works
- [ ] Security features work

## Issues Found
[List any issues encountered during testing]

## Test Environment
- VM Type: [QEMU/VirtualBox]
- RAM: 2048 MB
- Disk: 20 GB
- ISO Version: [version]
- Test Date: [date]
EOF

echo -e "${GREEN}=== Test VM Setup Complete ===${NC}"
echo ""
echo "Test VMs are ready for ISO installation testing."
echo "Test checklist created at: $VM_DIR/TEST-CHECKLIST.md"
echo ""
echo "Next steps:"
echo "  1. Build ISO: ./scripts/build-iso.sh"
echo "  2. Launch test VM: $VM_DIR/qemu/launch-vm.sh [path-to-iso]"
echo "  3. Follow test checklist: $VM_DIR/TEST-CHECKLIST.md"



# MilOS Test VM Setup Script
# Sets up test VMs for ISO installation testing

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
VM_DIR="$PROJECT_ROOT/test-vms"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== MilOS Test VM Setup ===${NC}"
echo "VM Directory: $VM_DIR"
echo ""

# Check prerequisites
check_command() {
    if ! command -v "$1" &> /dev/null; then
        echo -e "${YELLOW}Warning: $1 is not installed${NC}"
        return 1
    fi
    return 0
}

echo -e "${YELLOW}Checking prerequisites...${NC}"

HAS_QEMU=false
HAS_VBOX=false

if check_command qemu-system-x86_64; then
    HAS_QEMU=true
    echo -e "${GREEN}✓ QEMU/KVM available${NC}"
fi

if check_command VBoxManage; then
    HAS_VBOX=true
    echo -e "${GREEN}✓ VirtualBox available${NC}"
fi

if [ "$HAS_QEMU" = false ] && [ "$HAS_VBOX" = false ]; then
    echo -e "${RED}Error: No virtualization software found${NC}"
    echo "Please install QEMU/KVM or VirtualBox"
    exit 1
fi

echo ""

# Create VM directory
mkdir -p "$VM_DIR"

# QEMU/KVM VM setup
if [ "$HAS_QEMU" = true ]; then
    echo -e "${YELLOW}Setting up QEMU/KVM test VM...${NC}"
    
    QEMU_VM_DIR="$VM_DIR/qemu"
    mkdir -p "$QEMU_VM_DIR"
    
    # Create VM disk image (20GB)
    if [ ! -f "$QEMU_VM_DIR/milos-test-disk.qcow2" ]; then
        echo "Creating QEMU disk image (20GB)..."
        qemu-img create -f qcow2 "$QEMU_VM_DIR/milos-test-disk.qcow2" 20G
        echo -e "${GREEN}✓ QEMU disk image created${NC}"
    else
        echo -e "${YELLOW}QEMU disk image already exists${NC}"
    fi
    
    # Create VM launch script
    cat > "$QEMU_VM_DIR/launch-vm.sh" << 'EOF'
#!/bin/bash
# Launch MilOS test VM with QEMU

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ISO_PATH="${1:-../../build-iso/milos-*.iso}"

if [ ! -f "$ISO_PATH" ]; then
    echo "Error: ISO file not found: $ISO_PATH"
    echo "Usage: $0 [path-to-iso]"
    exit 1
fi

qemu-system-x86_64 \
    -cdrom "$ISO_PATH" \
    -hda "$SCRIPT_DIR/milos-test-disk.qcow2" \
    -m 2048 \
    -smp 2 \
    -boot d \
    -enable-kvm \
    -vga virtio \
    -display gtk
EOF
    
    chmod +x "$QEMU_VM_DIR/launch-vm.sh"
    echo -e "${GREEN}✓ QEMU VM setup complete${NC}"
    echo "  Launch with: $QEMU_VM_DIR/launch-vm.sh [path-to-iso]"
    echo ""
fi

# VirtualBox VM setup
if [ "$HAS_VBOX" = true ]; then
    echo -e "${YELLOW}Setting up VirtualBox test VM...${NC}"
    
    VM_NAME="MilOS-Test"
    
    # Check if VM already exists
    if VBoxManage list vms | grep -q "$VM_NAME"; then
        echo -e "${YELLOW}VirtualBox VM '$VM_NAME' already exists${NC}"
        echo "  To recreate, delete it first: VBoxManage unregistervm '$VM_NAME' --delete"
    else
        echo "Creating VirtualBox VM..."
        
        # Create VM
        VBoxManage createvm --name "$VM_NAME" --register --ostype "Linux_64"
        
        # Configure VM
        VBoxManage modifyvm "$VM_NAME" \
            --memory 2048 \
            --cpus 2 \
            --vram 128 \
            --boot1 dvd \
            --boot2 disk \
            --boot3 none \
            --boot4 none \
            --acpi on \
            --ioapic on \
            --pae on \
            --longmode on \
            --largepages on \
            --paravirtprovider kvm
        
        # Create disk
        VBOX_DISK="$VM_DIR/vbox/milos-test-disk.vdi"
        mkdir -p "$VM_DIR/vbox"
        VBoxManage createhd --filename "$VBOX_DISK" --size 20480 --format VDI
        
        # Attach disk
        VBoxManage storagectl "$VM_NAME" --name "SATA" --add sata --controller IntelAHCI
        VBoxManage storageattach "$VM_NAME" --storagectl "SATA" --port 0 --device 0 --type hdd --medium "$VBOX_DISK"
        
        # Attach DVD (for ISO)
        VBoxManage storagectl "$VM_NAME" --name "IDE" --add ide
        echo -e "${GREEN}✓ VirtualBox VM created${NC}"
        echo "  To attach ISO: VBoxManage storageattach '$VM_NAME' --storagectl IDE --port 0 --device 0 --type dvddrive --medium [path-to-iso]"
        echo "  To start VM: VBoxManage startvm '$VM_NAME'"
    fi
    echo ""
fi

# Create test checklist
cat > "$VM_DIR/TEST-CHECKLIST.md" << 'EOF'
# MilOS ISO Installation Test Checklist

## Pre-Installation
- [ ] ISO boots successfully
- [ ] GRUB menu appears
- [ ] Live mode works (if available)
- [ ] Installer launches

## Installation Process
- [ ] Welcome screen displays
- [ ] Partitioning works (auto and manual)
- [ ] Encryption setup works
- [ ] User account creation works
- [ ] Network configuration works
- [ ] Hardware detection works
- [ ] Package installation completes
- [ ] Installation completes successfully

## Post-Installation
- [ ] System boots from installed disk
- [ ] All services start correctly
- [ ] D-Bus interfaces work
- [ ] GUI applications launch
- [ ] CLI tools work
- [ ] Network connectivity works
- [ ] Security features work

## Issues Found
[List any issues encountered during testing]

## Test Environment
- VM Type: [QEMU/VirtualBox]
- RAM: 2048 MB
- Disk: 20 GB
- ISO Version: [version]
- Test Date: [date]
EOF

echo -e "${GREEN}=== Test VM Setup Complete ===${NC}"
echo ""
echo "Test VMs are ready for ISO installation testing."
echo "Test checklist created at: $VM_DIR/TEST-CHECKLIST.md"
echo ""
echo "Next steps:"
echo "  1. Build ISO: ./scripts/build-iso.sh"
echo "  2. Launch test VM: $VM_DIR/qemu/launch-vm.sh [path-to-iso]"
echo "  3. Follow test checklist: $VM_DIR/TEST-CHECKLIST.md"


