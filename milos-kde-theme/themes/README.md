# MilOS KDE Plasma Color Schemes

This directory contains custom KDE Plasma color schemes for MilOS, designed to provide different visual styles while maintaining the military-grade aesthetic and accessibility standards.

## Available Themes

### 1. MilOS Pitch Black (`MilOS-PitchBlack.colors`)

**Style:** Military-grade pitch-black theme  
**Description:** An ultra-dark theme with true black (#000000) backgrounds and high-contrast white text. Designed for military command center aesthetics with minimal distractions and maximum focus.

**Color Characteristics:**
- **Background:** Pure black (#000000) for maximum contrast
- **Text:** Bright white (#FFFFFF) for optimal readability
- **Accents:** Xenon cyan (#00d4ff) for interactive elements
- **Status Colors:** Military green, tactical amber, critical red maintained

**Best For:**
- Extended operational use
- Low-light environments
- Maximum focus and minimal eye strain
- Military command center aesthetic

**Installation:**
```bash
# Copy to user color schemes directory
cp MilOS-PitchBlack.colors ~/.local/share/color-schemes/

# Or install system-wide (requires root)
sudo cp MilOS-PitchBlack.colors /usr/share/color-schemes/
```

**Usage:**
1. Open KDE System Settings
2. Go to Appearance → Colors
3. Select "MilOS Pitch Black" from the color scheme list
4. Apply the theme

---

### 2. MilOS Plasma Yellow (`MilOS-PlasmaYellow.colors`)

**Style:** Modern plasma yellow theme  
**Description:** A sleek dark theme with vibrant yellow/gold accents. Features a modern, high-tech aesthetic with plasma yellow highlights for a cool, futuristic look.

**Color Characteristics:**
- **Background:** Very dark gray (#0F0F0F) for depth
- **Text:** Bright white (#FFFFFF) for clarity
- **Accents:** Plasma yellow/gold (#FFD700, #FFEB3B) for highlights
- **Interactive Elements:** Golden yellow glow effects
- **Status Colors:** Enhanced with yellow accents

**Best For:**
- Modern, high-tech aesthetic
- Visual appeal and style
- Futuristic interface design
- Cool, contemporary look

**Installation:**
```bash
# Copy to user color schemes directory
cp MilOS-PlasmaYellow.colors ~/.local/share/color-schemes/

# Or install system-wide (requires root)
sudo cp MilOS-PlasmaYellow.colors /usr/share/color-schemes/
```

**Usage:**
1. Open KDE System Settings
2. Go to Appearance → Colors
3. Select "MilOS Plasma Yellow" from the color scheme list
4. Apply the theme

---

## Theme Details

### Color Scheme Structure

Both themes follow the KDE Plasma color scheme format with the following sections:

- **[Colors:Button]** - Button widget colors
- **[Colors:Complementary]** - Complementary UI element colors
- **[Colors:Header]** - Header/title bar colors
- **[Colors:Selection]** - Selected item colors
- **[Colors:Tooltip]** - Tooltip colors
- **[Colors:View]** - View/list colors
- **[Colors:Window]** - Window background colors
- **[ColorEffects]** - Color effects for disabled/inactive states
- **[WM]** - Window manager colors

### Accessibility

Both themes maintain:
- **WCAG AA compliance** - Minimum 4.5:1 contrast ratio for normal text
- **High contrast** - Optimized for readability
- **Color-blind friendly** - Status colors supplemented with icons/shapes
- **Extended use** - Designed for prolonged operational use

### Integration with MilOS Components

These color schemes work seamlessly with:
- MilOS Component Library xenon effects
- KDE Plasma widgets
- All MilOS security tools
- Custom QML components

The xenon effects in MilOS components will adapt to the theme:
- **Pitch Black:** Xenon cyan accents on pure black
- **Plasma Yellow:** Xenon yellow/gold accents on dark gray

---

## Customization

To customize these themes:

1. **Edit the `.colors` file** directly
2. **Color values** are in RGB format (0-255)
3. **Test changes** by applying the theme in System Settings
4. **Save custom versions** with new names

### Color Reference

**Pitch Black Theme:**
- Background: `0,0,0` (pure black)
- Text: `255,255,255` (white)
- Accent: `0,212,255` (xenon cyan)

**Plasma Yellow Theme:**
- Background: `15,15,15` (very dark gray)
- Text: `255,255,255` (white)
- Accent: `255,215,0` (golden yellow)

---

## Development

These color schemes are part of the MilOS KDE theme package and are installed automatically when building MilOS.

For development:
```bash
# Build and install themes
cd milos-kde-theme/themes
make install  # (if Makefile exists)
# Or manually copy to color-schemes directory
```

---

## License

Part of the MilOS project.

