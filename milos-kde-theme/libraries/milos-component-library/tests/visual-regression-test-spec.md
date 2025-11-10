# Visual Regression Test Specification

This document specifies the visual regression tests for component library components to ensure they match design specifications exactly.

## Test Methodology

Visual regression tests compare component rendering against design specifications from `component-library-design-system.md`. Tests can be performed manually or using automated screenshot comparison tools.

## Component 1: Button

### Variants Visual Tests
- [ ] **Primary Button**: Large, prominent xenon glow (Cyan #00d4ff, 4px blur, 80% opacity)
- [ ] **Secondary Button**: Medium, subtle xenon accent
- [ ] **Tertiary Button**: Small, minimal xenon glow
- [ ] **Icon Button**: Icon-only with xenon glow on hover
- [ ] **Toggle Button**: Two-state button with xenon status indicator

### States Visual Tests
- [ ] **Default**: Base xenon glow (intensity: standard)
- [ ] **Hover**: Increased xenon glow intensity (150% base intensity)
- [ ] **Active/Pressed**: Xenon pulse animation with glow intensification (2s duration)
- [ ] **Disabled**: Reduced opacity (30%) with minimal xenon glow
- [ ] **Loading/Processing**: Xenon scanning animation across button
- [ ] **Success**: Xenon green glow (#00cc66) with celebration pulse (3 quick pulses)
- [ ] **Error**: Xenon red glow (#ff4444) with alert pulse

### Accessibility Visual Tests
- [ ] **Minimum Height**: 48px minimum height verified
- [ ] **Focus Indicator**: Keyboard focus shows xenon border glow (2px solid, Cyan #00d4ff)
- [ ] **Color Contrast**: Text meets WCAG AA standards (4.5:1 minimum)

## Component 2: Input Field

### Variants Visual Tests
- [ ] **Text Input**: Standard single-line text input with xenon border
- [ ] **Password Input**: Masked input with xenon strength indicator
- [ ] **Textarea**: Multi-line text input
- [ ] **Search Input**: Search field with xenon search icon
- [ ] **Number Input**: Numeric input with xenon-accented increment/decrement

### States Visual Tests
- [ ] **Default**: Xenon border glow (Cyan #00d4ff, 1px, 40% opacity)
- [ ] **Focus**: Xenon border glow intensifies (Cyan #00ffff, 2px, 80% opacity, pulse)
- [ ] **Filled**: Xenon glow shifts to cyan when content valid
- [ ] **Error**: Xenon red border glow (#ff4444, 2px, pulsing alert)
- [ ] **Disabled**: Reduced opacity with minimal xenon accent
- [ ] **Read-only**: Xenon gray border, no interaction glow

### Password Strength Indicator Visual Tests
- [ ] **Weak Password**: Red gradient (#ff4444)
- [ ] **Medium Password**: Amber gradient (#ffaa00)
- [ ] **Strong Password**: Green gradient (#00cc66)
- [ ] **Gradient Transition**: Smooth color transition (red → amber → green)

### Accessibility Visual Tests
- [ ] **Label Position**: Input labels positioned above with xenon-accented styling
- [ ] **Focus Indicator**: Keyboard focus shows xenon border glow (2px solid)
- [ ] **Color Contrast**: Text meets WCAG AA standards

## Component 3: Status Indicator

### Variants Visual Tests
- [ ] **Status Badge**: Small status indicator with xenon glow
- [ ] **Status Icon**: Icon-based status with xenon accent
- [ ] **Status Light**: Circular indicator with xenon pulsing glow
- [ ] **Status Card**: Large status display card with xenon border
- [ ] **Status Widget**: Panel widget with persistent status display

### States Visual Tests
- [ ] **Active/Secure**: Xenon green glow (#00cc66, 8px blur, 2s pulse)
- [ ] **Inactive/Insecure**: Xenon red/gray indicator
- [ ] **Warning/Caution**: Xenon amber glow (#ffaa00, 6px blur, 3s pulse)
- [ ] **Critical/Alert**: Xenon red glow (#ff4444, 10px blur, 1s pulse)
- [ ] **Processing/Connecting**: Xenon blue scanning animation (#00d4ff, scanning line with trail)

### Accessibility Visual Tests
- [ ] **Status Text**: Status text accompanies visual indicators
- [ ] **Non-Color Reliance**: Icons, shapes, text accompany color coding
- [ ] **Screen Reader**: Status changes trigger screen reader announcements

## Component 4: Progress Bar

### Variants Visual Tests
- [ ] **Linear Progress Bar**: Horizontal progress bar with xenon fill
- [ ] **Circular Progress**: Circular progress indicator with xenon animation
- [ ] **Step Progress**: Multi-step progress with xenon-accented steps
- [ ] **Indeterminate Progress**: Animated progress when duration unknown
- [ ] **Progress with Details**: Progress bar with text details and xenon counters

### States Visual Tests
- [ ] **In Progress**: Xenon pulse animation along progress bar (Cyan #00d4ff)
- [ ] **Paused**: Xenon amber indicator (#ffaa00) with paused animation
- [ ] **Complete**: Xenon green glow (#00cc66) with celebration pulse (3 quick pulses)
- [ ] **Error**: Xenon red indicator (#ff4444) with alert pulse
- [ ] **Indeterminate**: Xenon scanning animation

### Progress Effects Visual Tests
- [ ] **Progress Fill**: Cyan (#00d4ff) pulse animation traveling along bar
- [ ] **Progress Trail**: Xenon particle effects following progress fill
- [ ] **Complete Glow**: Green (#00cc66) celebration pulse with 3 quick pulses
- [ ] **Error Alert**: Red (#ff4444) pulsing alert animation

### Accessibility Visual Tests
- [ ] **Progress Percentage**: Progress percentage displayed with xenon-accented text
- [ ] **Estimated Time**: Estimated time remaining shown
- [ ] **Minimum Height**: 8px minimum height for visibility
- [ ] **Screen Reader**: Progress updates announced to screen readers

## Integration Visual Tests

### Encryption Manager Integration
- [ ] **Button Components**: Buttons render correctly in Encryption Setup Screen
- [ ] **Input Field Components**: Input fields render correctly with password strength indicator
- [ ] **Progress Bar Components**: Progress bar displays correctly during encryption
- [ ] **Status Indicator Components**: Status indicators show password match status correctly
- [ ] **No Visual Regressions**: Existing UI elements unchanged after component integration

### Network Configuration Screen Integration
- [ ] **Status Indicator Components**: Status indicators show network connection status correctly
- [ ] **Button Components**: Buttons render correctly for network configuration actions
- [ ] **Input Field Components**: Input fields render correctly for IP configuration
- [ ] **No Visual Regressions**: Existing UI elements unchanged after component integration

## Color Palette Verification

### Xenon Colors
- [ ] **Xenon Primary**: #00d4ff (default xenon glow)
- [ ] **Xenon Secondary**: #00ffff (hover states)
- [ ] **Xenon Success**: #00cc66 (success states)
- [ ] **Xenon Warning**: #ffaa00 (warning states)
- [ ] **Xenon Critical**: #ff4444 (error states)

### Background Colors
- [ ] **Background Primary**: #0a0a0a (main application backgrounds)
- [ ] **Background Secondary**: #1a1a1a (card backgrounds, panels)
- [ ] **Background Tertiary**: #2a2a2a (input fields, dropdowns)

### Text Colors
- [ ] **Text Primary**: #ffffff (primary text content)
- [ ] **Text Secondary**: #e0e0e0 (secondary text, labels)
- [ ] **Text Tertiary**: #b0b0b0 (disabled text, hints)

## Animation Verification

### Animation 1: Xenon Glow Pulse
- [ ] **Duration**: 2000ms (2 seconds) continuous loop
- [ ] **Easing**: Ease-in-out (smooth acceleration/deceleration)
- [ ] **Effect**: Cyan (#00d4ff) glow intensity pulses from 60% to 100% opacity

### Animation 2: Xenon Border Scan
- [ ] **Duration**: Continuous (until operation completes)
- [ ] **Easing**: Linear (consistent speed)
- [ ] **Effect**: Cyan (#00d4ff) scanning line travels along progress bar with trail effect

### Animation 4: Xenon Alert Pulse
- [ ] **Duration**: 1000ms (1 second) continuous loop
- [ ] **Easing**: Ease-in-out with bounce effect
- [ ] **Effect**: Red (#ff4444) glow pulses intensely from 80% to 100% opacity

### Animation 5: Xenon Success Celebration
- [ ] **Duration**: 1500ms (1.5 seconds) one-time animation
- [ ] **Easing**: Ease-out with bounce (celebration feel)
- [ ] **Effect**: Green (#00cc66) glow intensifies with 3 quick pulses, then settles

### Animation 9: Xenon Focus Flash
- [ ] **Duration**: 200ms (0.2 seconds)
- [ ] **Easing**: Ease-out (quick response)
- [ ] **Effect**: Cyan (#00ffff) border glow flashes with 2px width, 80% opacity

## Typography Verification

- [ ] **Primary Font**: Noto Sans (body text, labels, UI elements)
  - [ ] Regular (400), Medium (500), Bold (700)
  - [ ] Body text: 16px minimum, 1.5 line height
  - [ ] Labels: 14px, 500 weight, 1.4 line height
- [ ] **Secondary Font**: JetBrains Mono (technical data, code, file paths)
  - [ ] Regular (400)
  - [ ] Size: 14px, 1.5 line height

## Spacing Verification

- [ ] **Base Unit**: 4px (all spacing multiples of 4)
- [ ] **Scale Values**: XXS (4px), XS (8px), SM (12px), MD (16px), LG (24px), XL (32px), XXL (48px)
- [ ] **Component Padding**: 16px (MD) standard, 24px (LG) for cards
- [ ] **Component Gaps**: 12px (SM) small, 16px (MD) standard, 24px (LG) large

## Test Execution

1. **Manual Visual Inspection**: Review each component variant and state visually
2. **Screenshot Comparison**: Capture screenshots and compare against design specifications
3. **Color Verification**: Use color picker tools to verify hex color codes
4. **Animation Verification**: Observe animations and verify timing matches specifications
5. **Accessibility Testing**: Verify accessibility features are visually apparent

## Test Results

Visual regression tests should be performed before each release to ensure components match design specifications exactly. Any deviations should be documented and addressed.

