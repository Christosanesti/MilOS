# MilOS Installer Test Suite

## Overview

This test suite provides comprehensive testing for the MilOS installer application, covering:
- Welcome screen rendering and functionality
- Partitioning screen functionality
- Wizard navigation
- Installation state management
- Accessibility compliance (WCAG AA)
- Component library integration
- Xenon effects integration

## Test Structure

### Unit Tests

- `test_welcomescreen.cpp` - Welcome screen tests
- `test_partitioningscreen.cpp` - Partitioning screen tests
- `test_wizardnavigation.cpp` - Wizard navigation tests
- `test_installerstate.cpp` - Installation state management tests

### Integration Tests

- `test_integration.cpp` - Component library and xenon effects integration tests

### Accessibility Tests

- `test_accessibility.cpp` - WCAG AA accessibility compliance tests

## Running Tests

### Build Tests

```bash
cd milos-installer
mkdir -p build
cd build
cmake ..
make milos-installer-tests
```

### Run Tests

```bash
cd build
./milos-installer-tests
```

Or using CTest:

```bash
cd build
ctest
```

## Test Coverage

### Welcome Screen Tests
- Welcome screen rendering
- Logo pulsing animation (Animation 1: Xenon Glow Pulse)
- Language selection functionality
- Button rendering and hover effects
- Keyboard navigation
- Screen reader support

### Partitioning Screen Tests
- Partitioning screen rendering
- Disk detection
- Auto-partition option
- Recommended badge
- Manual partition editor
- Warning dialogs
- Keyboard navigation
- Screen reader support

### Wizard Navigation Tests
- Wizard navigation system
- Step indicator
- Back/Next buttons
- Step validation
- State persistence
- State recovery
- Checkpoint system
- Keyboard navigation

### Accessibility Tests
- WCAG AA color contrast (4.5:1 minimum)
- Keyboard navigation
- Touch targets (44x44px minimum)
- Screen reader support
- Focus indicators
- Non-color reliance

### Integration Tests
- Component library integration (Components 1, 3, 6, 8)
- Xenon effects library integration
- Wizard navigation integration
- State persistence integration

## Visual Regression Tests

Visual regression tests comparing installer to design specifications should be performed manually or using automated visual testing tools. These tests verify:
- Component rendering matches design specifications
- Xenon effects match animation specifications
- Color palette matches branding guide
- Typography matches style guide
- Spacing matches layout specifications

## Notes

- Tests require Qt 6.x and MilOS component library to be built
- Some tests may require runtime environment setup (D-Bus, component library paths)
- Visual regression tests require manual verification or automated visual testing tools
- Integration tests verify component library and xenon effects are properly integrated

