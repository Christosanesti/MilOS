#!/bin/bash
# MilOS ISO Build Prerequisites Installation Script
# Installs required packages for ISO building

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== MilOS ISO Build Prerequisites Installer ===${NC}"
echo ""

# Detect package manager
if command -v pacman &> /dev/null; then
    PKG_MANAGER="pacman"
    INSTALL_CMD="sudo pacman -S --noconfirm"
    PKGS=(
        "genisoimage"      # Provides mkisofs
        "grub"             # Provides grub-mkrescue
        "libisoburn"       # Provides xorriso
        "pacman"            # Package manager (should already be installed)
        "gnupg"             # Provides gpg
        "cmake"
        "make"
        "gcc"
        "pkg-config"
        "qt5-base"
        "qt5-declarative"
        "sqlite"
    )
elif command -v apt-get &> /dev/null; then
    PKG_MANAGER="apt"
    INSTALL_CMD="sudo apt-get install -y"
    PKGS=(
        "genisoimage"      # Provides mkisofs
        "grub-pc-bin"      # Provides grub-mkrescue
        "xorriso"          # Direct package
        "pacman"           # May not be available on Debian/Ubuntu
        "gnupg"            # Provides gpg
        "cmake"
        "make"
        "gcc"
        "pkg-config"
        "qt5-default"
        "libqt5sql5-sqlite"
    )
elif command -v yum &> /dev/null || command -v dnf &> /dev/null; then
    PKG_MANAGER="yum/dnf"
    if command -v dnf &> /dev/null; then
        INSTALL_CMD="sudo dnf install -y"
    else
        INSTALL_CMD="sudo yum install -y"
    fi
    PKGS=(
        "genisoimage"      # Provides mkisofs
        "grub2-tools"      # Provides grub-mkrescue
        "xorriso"          # Direct package
        "gnupg2"           # Provides gpg
        "cmake"
        "make"
        "gcc"
        "pkgconfig"
        "qt5-qtbase"
        "qt5-qtdeclarative"
        "sqlite"
    )
else
    echo -e "${RED}✗ Unsupported package manager${NC}"
    echo "Please install the following packages manually:"
    echo "  - mkisofs (genisoimage package)"
    echo "  - grub-mkrescue (grub package)"
    echo "  - xorriso (libisoburn/xorriso package)"
    echo "  - pacman (if building on Arch-based system)"
    echo "  - gpg (gnupg package)"
    echo "  - cmake, make, gcc, pkg-config"
    echo "  - Qt5 development packages"
    echo "  - sqlite"
    exit 1
fi

echo -e "${YELLOW}Detected package manager: ${PKG_MANAGER}${NC}"
echo ""

# Check if running as root (for some operations)
if [ "$EUID" -eq 0 ]; then
    echo -e "${YELLOW}Warning: Running as root. Some operations may not work correctly.${NC}"
    echo ""
fi

# Check which packages are already installed
echo -e "${YELLOW}Checking installed packages...${NC}"
MISSING_PKGS=()

for pkg in "${PKGS[@]}"; do
    # Simple check - may need adjustment per package manager
    if [ "$PKG_MANAGER" = "pacman" ]; then
        if pacman -Qi "$pkg" &> /dev/null; then
            echo -e "${GREEN}✓ $pkg is installed${NC}"
        else
            echo -e "${RED}✗ $pkg is missing${NC}"
            MISSING_PKGS+=("$pkg")
        fi
    elif [ "$PKG_MANAGER" = "apt" ]; then
        if dpkg -l | grep -q "^ii.*$pkg"; then
            echo -e "${GREEN}✓ $pkg is installed${NC}"
        else
            echo -e "${RED}✗ $pkg is missing${NC}"
            MISSING_PKGS+=("$pkg")
        fi
    else
        # For yum/dnf, just add to missing list
        MISSING_PKGS+=("$pkg")
    fi
done

echo ""

if [ ${#MISSING_PKGS[@]} -eq 0 ]; then
    echo -e "${GREEN}✓ All required packages are already installed!${NC}"
    exit 0
fi

# Ask for confirmation
echo -e "${YELLOW}The following packages need to be installed:${NC}"
for pkg in "${MISSING_PKGS[@]}"; do
    echo "  - $pkg"
done
echo ""
read -p "Do you want to install these packages? (y/N): " -n 1 -r
echo ""

if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo -e "${YELLOW}Installation cancelled.${NC}"
    exit 0
fi

# Install missing packages
echo -e "${BLUE}Installing packages...${NC}"
$INSTALL_CMD "${MISSING_PKGS[@]}"

if [ $? -eq 0 ]; then
    echo ""
    echo -e "${GREEN}✓ All packages installed successfully!${NC}"
    echo ""
    echo "You can now run: ./scripts/test-iso-build.sh"
else
    echo ""
    echo -e "${RED}✗ Some packages failed to install${NC}"
    echo "Please install them manually and try again."
    exit 1
fi

