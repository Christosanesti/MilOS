cmake_minimum_required(VERSION 3.27)

# MilOS KDE Plasma Color Schemes Installation
# This CMakeLists.txt installs custom MilOS color schemes to the KDE color-schemes directory

# Install color scheme files
install(FILES
    MilOS-PitchBlack.colors
    MilOS-PlasmaYellow.colors
    DESTINATION ${CMAKE_INSTALL_DATADIR}/color-schemes
)

# Install README documentation
install(FILES
    README.md
    DESTINATION ${CMAKE_INSTALL_DATADIR}/color-schemes/milos-themes
    RENAME README.md
)

