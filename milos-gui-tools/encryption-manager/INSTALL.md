# Qt6 Installation Instructions

## Ubuntu/Debian Installation

To build the Encryption Manager, you need Qt6 development packages installed:

```bash
sudo apt-get update
sudo apt-get install -y \
    qt6-base-dev \
    qt6-base-dev-tools \
    qt6-tools-dev \
    cmake \
    build-essential
```

## Verify Installation

After installation, verify Qt6 is available:

```bash
cmake --version
qmake6 --version
```

## Build Instructions

Once Qt6 is installed, build the project:

```bash
cd milos-gui-tools/encryption-manager
mkdir -p build
cd build
cmake ..
make
```

## Alternative: Qt6 Online Installer

If you prefer to install Qt6 manually or need a specific version:

1. Download Qt6 Online Installer from: https://www.qt.io/download-qt-installer
2. Install Qt6 to a local directory (e.g., `~/Qt/6.8.0/`)
3. Set CMAKE_PREFIX_PATH when configuring:

```bash
cmake -DCMAKE_PREFIX_PATH=~/Qt/6.8.0/gcc_64 ..
```

## Requirements

- Qt6 version 6.8.x or later
- CMake 3.27 or later
- C++20 compatible compiler (GCC 13.2+ or Clang 17.0+)

