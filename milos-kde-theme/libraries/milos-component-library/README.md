# MilOS Component Library

A comprehensive Qt/QML component library for MilOS security tools, featuring xenon effects, full accessibility compliance, and consistent branding.

## Overview

The MilOS Component Library provides 11 reusable UI components designed for security tools, all following the MilOS design system with xenon visual effects, WCAG AA accessibility compliance, and consistent branding.

## Components

### Base Components
1. **Button** - 5 variants (Primary, Secondary, Tertiary, Icon, Toggle), 7 states, xenon effects
2. **Input Field** - 5 variants (Text, Password, Textarea, Search, Number), 6 states, xenon border effects
3. **Status Indicator** - 5 variants (Badge, Icon, Light, Card, Widget), 5 states, xenon glow effects
4. **Progress Bar** - 5 variants (Linear, Circular, Step, Indeterminate, Details), 5 states, xenon animations

### Navigation & Feedback Components
5. **Tab Navigation** - 5 variants (Standard, Icon, Pill, Underline, Vertical), 5 states, xenon transitions
6. **Notification/Alert** - 6 variants (Success, Error, Warning, Info, Security, Toast), 4 states, xenon slide-in animations
7. **Card** - 5 variants (Status, Dashboard, Interactive, Detail, Alert), 5 states, xenon border effects

### Advanced Components
8. **Dropdown Menu** - 5 variants (Standard, Multi-Select, Searchable, Icon, Context), 5 states, xenon effects
9. **Data Visualization** - 5 variants (Line Chart, Bar Chart, Area Chart, Network Graph, Real-time Stream), 5 states, xenon particle effects
10. **Modal Dialog** - 5 variants (Confirmation, Warning, Error, Form, Progress), 4 states, xenon border effects

### KDE Integration
11. **Plasma Widget** - 5 variants (Security Posture, Network Monitor, Encryption Status, Quick Actions, Emergency Actions), 4 states, xenon effects

## Features

- **Xenon Effects**: All components feature xenon visual effects (glow, pulse, animations)
- **Accessibility**: Full WCAG AA compliance with keyboard navigation, screen reader support, touch targets
- **Branding**: Consistent color palette, typography, and spacing following MilOS style guide
- **Animations**: Smooth animations following MilOS animation specifications
- **Dual API**: Components available as both C++ classes and QML components

## Usage

### QML Usage

```qml
import MilosComponents 1.0

Button {
    variant: "primary"
    text: "Click Me"
    onClicked: {
        // Handle click
    }
}
```

### C++ Usage

```cpp
#include <MilosComponents/Button.h>

Button *button = new Button();
button->setVariant("primary");
button->setText("Click Me");
```

## Building

```bash
mkdir build
cd build
cmake ..
make
sudo make install
```

## Requirements

- Qt 6.x (Qt 6.8.x series minimum)
- KDE Frameworks 6.x
- CMake 3.27+
- C++20 compiler (GCC 13.2+ / Clang 17.0+)

## Documentation

Comprehensive component library documentation is available in the `docs/` directory:

- **[API.md](docs/API.md)** - Complete API documentation for all 11 components
- **[UsageGuidelines.md](docs/UsageGuidelines.md)** - When and how to use each component
- **[IntegrationPatterns.md](docs/IntegrationPatterns.md)** - Integration patterns and examples
- **[CustomizationGuidelines.md](docs/CustomizationGuidelines.md)** - How to customize components

### Quick Start

1. **Import the module:**
   ```qml
   import MilosComponents 1.0
   ```

2. **Use components:**
   ```qml
   Button {
       text: "Click Me"
       variant: "primary"
       onClicked: {
           // Handle click
       }
   }
   ```

3. **Configure import path in your application:**
   ```cpp
   QStringList importPaths = engine.importPathList();
   importPaths.prepend(componentLibraryPath);
   engine.setImportPathList(importPaths);
   ```

## License

Part of the MilOS project.

