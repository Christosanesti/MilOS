cmake_minimum_required(VERSION 3.27)
project(MilosKdeTheme VERSION 1.0.0 LANGUAGES CXX)

# MilOS KDE Theme Package
# This package includes:
# - MilOS Component Library (Qt/QML components with xenon effects)
# - MilOS KDE Plasma Color Schemes (custom themes)

# Add component library subdirectory
add_subdirectory(libraries/milos-component-library)

# Add themes subdirectory
add_subdirectory(themes)

