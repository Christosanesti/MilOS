#include "accessibility.h"
#include <QDebug>
#include <QGuiApplication>
#include <QAccessible>
#include <QAccessibleInterface>
#include <cmath>

Accessibility::Accessibility(QObject* parent)
    : QObject(parent)
    , m_wcagCompliant(true)
    , m_keyboardNavigationEnabled(true)
    , m_screenReaderEnabled(false)
{
    detectScreenReader();
    verifyWCAGCompliance();
}

Accessibility::~Accessibility()
{
}

void Accessibility::setKeyboardNavigationEnabled(bool enabled)
{
    if (m_keyboardNavigationEnabled != enabled) {
        m_keyboardNavigationEnabled = enabled;
        emit keyboardNavigationEnabledChanged();
    }
}

bool Accessibility::checkColorContrast(const QColor& foreground, const QColor& background)
{
    return meetsWCAGAA(foreground, background);
}

double Accessibility::getContrastRatio(const QColor& foreground, const QColor& background)
{
    double l1 = calculateLuminance(foreground);
    double l2 = calculateLuminance(background);
    
    double lighter = qMax(l1, l2);
    double darker = qMin(l1, l2);
    
    return (lighter + 0.05) / (darker + 0.05);
}

bool Accessibility::isColorBlindFriendly(const QColor& color1, const QColor& color2)
{
    // Check if colors are distinguishable beyond just color
    // Use luminance difference as a proxy
    double l1 = calculateLuminance(color1);
    double l2 = calculateLuminance(color2);
    
    // Colors should have sufficient luminance difference
    return qAbs(l1 - l2) > 0.2;
}

void Accessibility::verifyWCAGCompliance()
{
    // Verify common color combinations meet WCAG AA
    // This is a simplified check - full compliance requires testing all UI elements
    
    // High-contrast mode colors
    QColor text = QColor("#000000");
    QColor background = QColor("#ffffff");
    
    bool compliant = checkColorContrast(text, background);
    
    if (m_wcagCompliant != compliant) {
        m_wcagCompliant = compliant;
        emit wcagCompliantChanged();
    }
    
    if (!compliant) {
        emit complianceIssueDetected("Color contrast does not meet WCAG AA standards");
    }
}

void Accessibility::detectScreenReader()
{
    // Check if screen reader is active
    QAccessibleInterface* root = QAccessible::queryAccessibleInterface(qApp);
    if (root) {
        // Check for screen reader presence
        // This is a simplified check - actual detection may vary
        m_screenReaderEnabled = QAccessible::isActive();
        emit screenReaderEnabledChanged();
    }
}

double Accessibility::calculateLuminance(const QColor& color)
{
    // Calculate relative luminance according to WCAG
    double r = color.redF();
    double g = color.greenF();
    double b = color.blueF();
    
    // Convert to linear RGB
    r = (r <= 0.03928) ? r / 12.92 : std::pow((r + 0.055) / 1.055, 2.4);
    g = (g <= 0.03928) ? g / 12.92 : std::pow((g + 0.055) / 1.055, 2.4);
    b = (b <= 0.03928) ? b / 12.92 : std::pow((b + 0.055) / 1.055, 2.4);
    
    // Calculate relative luminance
    return 0.2126 * r + 0.7152 * g + 0.0722 * b;
}

bool Accessibility::meetsWCAGAA(const QColor& foreground, const QColor& background)
{
    double ratio = getContrastRatio(foreground, background);
    // WCAG AA requires 4.5:1 for normal text, 3:1 for large text
    return ratio >= 4.5;
}

