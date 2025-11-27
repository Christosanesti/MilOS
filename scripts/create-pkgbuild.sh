#!/bin/bash
# MilOS PKGBUILD Generator
# Generates PKGBUILD files for all MilOS packages

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
PKGBUILD_DIR="$PROJECT_ROOT/pkgbuilds"

mkdir -p "$PKGBUILD_DIR"

# Template for PKGBUILD
create_pkgbuild() {
    local pkgname=$1
    local pkgver=$2
    local pkgdesc=$3
    local depends=$4
    local builddir=$5
    
    cat > "$PKGBUILD_DIR/PKGBUILD.$pkgname" << EOF
# Maintainer: MilOS Development Team <dev@milos.org>
pkgname=$pkgname
pkgver=$pkgver
pkgrel=1
pkgdesc="$pkgdesc"
arch=('x86_64')
url="https://milos.org"
license=('GPL3')
depends=($depends)
makedepends=('cmake' 'qt6-base' 'qt6-declarative' 'qt6-tools')
source=("\${pkgname}-\${pkgver}.tar.gz")
sha256sums=('SKIP')

build() {
    cd "\${srcdir}/\${pkgname}-\${pkgver}"
    mkdir -p build
    cd build
    cmake .. \\
        -DCMAKE_BUILD_TYPE=Release \\
        -DCMAKE_INSTALL_PREFIX=/usr
    make
}

package() {
    cd "\${srcdir}/\${pkgname}-\${pkgver}/build"
    make DESTDIR="\${pkgdir}" install
}
EOF
}

echo "Creating PKGBUILD files..."

# Create PKGBUILD for logging library
create_pkgbuild \
    "milos-logging" \
    "1.0.0" \
    "MilOS centralized logging library" \
    "'qt6-base' 'qt6-dbus'" \
    "milos-base/libs/logging"

# Create PKGBUILD for UI utilities
create_pkgbuild \
    "milos-ui-utilities" \
    "1.0.0" \
    "MilOS UI utilities and error handling" \
    "'qt6-base' 'qt6-dbus' 'qt6-network' 'qt6-opengl' 'milos-logging'" \
    "milos-base/libs/ui-utilities"

# Create PKGBUILDs for services
for service_dir in "$PROJECT_ROOT/milos-base/services"/*/; do
    if [ -f "$service_dir/CMakeLists.txt" ]; then
        service_name=$(basename "$service_dir")
        pkgname="milos-${service_name}"
        
        # Determine dependencies based on service type
        deps="'qt6-base' 'qt6-dbus' 'milos-logging' 'systemd'"
        
        # Add service-specific dependencies
        case "$service_name" in
            "tpm-integration")
                deps="$deps 'tpm2-tss' 'tpm2-tools'"
                ;;
            "file-integrity-monitoring")
                deps="$deps 'sqlite'"
                ;;
            "update-service")
                deps="$deps 'pacman' 'gpg'"
                ;;
        esac
        
        create_pkgbuild \
            "$pkgname" \
            "1.0.0" \
            "MilOS $service_name service" \
            "$deps" \
            "milos-base/services/$service_name"
    fi
done

# Create PKGBUILDs for GUI tools
for tool_dir in "$PROJECT_ROOT/milos-gui-tools"/*/; do
    if [ -f "$tool_dir/CMakeLists.txt" ]; then
        tool_name=$(basename "$tool_dir")
        pkgname="milos-${tool_name}"
        
        create_pkgbuild \
            "$pkgname" \
            "1.0.0" \
            "MilOS $tool_name GUI tool" \
            "'qt6-base' 'qt6-declarative' 'qt6-tools' 'milos-logging' 'milos-ui-utilities'" \
            "milos-gui-tools/$tool_name"
    fi
done

# Create PKGBUILD for installer
if [ -d "$PROJECT_ROOT/milos-installer" ]; then
    create_pkgbuild \
        "milos-installer" \
        "1.0.0" \
        "MilOS installation system" \
        "'qt6-base' 'qt6-declarative' 'qt6-tools' 'milos-logging' 'milos-ui-utilities'" \
        "milos-installer"
fi

echo ""
echo "PKGBUILD files created in $PKGBUILD_DIR"
echo "Total PKGBUILDs created: $(ls -1 "$PKGBUILD_DIR"/PKGBUILD.* 2>/dev/null | wc -l)"


# Generates PKGBUILD files for all MilOS packages

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
PKGBUILD_DIR="$PROJECT_ROOT/pkgbuilds"

mkdir -p "$PKGBUILD_DIR"

# Template for PKGBUILD
create_pkgbuild() {
    local pkgname=$1
    local pkgver=$2
    local pkgdesc=$3
    local depends=$4
    local builddir=$5
    
    cat > "$PKGBUILD_DIR/PKGBUILD.$pkgname" << EOF
# Maintainer: MilOS Development Team <dev@milos.org>
pkgname=$pkgname
pkgver=$pkgver
pkgrel=1
pkgdesc="$pkgdesc"
arch=('x86_64')
url="https://milos.org"
license=('GPL3')
depends=($depends)
makedepends=('cmake' 'qt6-base' 'qt6-declarative' 'qt6-tools')
source=("\${pkgname}-\${pkgver}.tar.gz")
sha256sums=('SKIP')

build() {
    cd "\${srcdir}/\${pkgname}-\${pkgver}"
    mkdir -p build
    cd build
    cmake .. \\
        -DCMAKE_BUILD_TYPE=Release \\
        -DCMAKE_INSTALL_PREFIX=/usr
    make
}

package() {
    cd "\${srcdir}/\${pkgname}-\${pkgver}/build"
    make DESTDIR="\${pkgdir}" install
}
EOF
}

echo "Creating PKGBUILD files..."

# Create PKGBUILD for logging library
create_pkgbuild \
    "milos-logging" \
    "1.0.0" \
    "MilOS centralized logging library" \
    "'qt6-base' 'qt6-dbus'" \
    "milos-base/libs/logging"

# Create PKGBUILD for UI utilities
create_pkgbuild \
    "milos-ui-utilities" \
    "1.0.0" \
    "MilOS UI utilities and error handling" \
    "'qt6-base' 'qt6-dbus' 'qt6-network' 'qt6-opengl' 'milos-logging'" \
    "milos-base/libs/ui-utilities"

# Create PKGBUILDs for services
for service_dir in "$PROJECT_ROOT/milos-base/services"/*/; do
    if [ -f "$service_dir/CMakeLists.txt" ]; then
        service_name=$(basename "$service_dir")
        pkgname="milos-${service_name}"
        
        # Determine dependencies based on service type
        deps="'qt6-base' 'qt6-dbus' 'milos-logging' 'systemd'"
        
        # Add service-specific dependencies
        case "$service_name" in
            "tpm-integration")
                deps="$deps 'tpm2-tss' 'tpm2-tools'"
                ;;
            "file-integrity-monitoring")
                deps="$deps 'sqlite'"
                ;;
            "update-service")
                deps="$deps 'pacman' 'gpg'"
                ;;
        esac
        
        create_pkgbuild \
            "$pkgname" \
            "1.0.0" \
            "MilOS $service_name service" \
            "$deps" \
            "milos-base/services/$service_name"
    fi
done

# Create PKGBUILDs for GUI tools
for tool_dir in "$PROJECT_ROOT/milos-gui-tools"/*/; do
    if [ -f "$tool_dir/CMakeLists.txt" ]; then
        tool_name=$(basename "$tool_dir")
        pkgname="milos-${tool_name}"
        
        create_pkgbuild \
            "$pkgname" \
            "1.0.0" \
            "MilOS $tool_name GUI tool" \
            "'qt6-base' 'qt6-declarative' 'qt6-tools' 'milos-logging' 'milos-ui-utilities'" \
            "milos-gui-tools/$tool_name"
    fi
done

# Create PKGBUILD for installer
if [ -d "$PROJECT_ROOT/milos-installer" ]; then
    create_pkgbuild \
        "milos-installer" \
        "1.0.0" \
        "MilOS installation system" \
        "'qt6-base' 'qt6-declarative' 'qt6-tools' 'milos-logging' 'milos-ui-utilities'" \
        "milos-installer"
fi

echo ""
echo "PKGBUILD files created in $PKGBUILD_DIR"
echo "Total PKGBUILDs created: $(ls -1 "$PKGBUILD_DIR"/PKGBUILD.* 2>/dev/null | wc -l)"

