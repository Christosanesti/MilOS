#ifndef ACCESSIBILITY_H
#define ACCESSIBILITY_H

#include <QObject>
#include <QString>
#include <QColor>

/**
 * @brief Accessibility Manager
 * 
 * Manages accessibility features and WCAG AA compliance
 */
class Accessibility : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool wcagCompliant READ wcagCompliant NOTIFY wcagCompliantChanged)
    Q_PROPERTY(bool keyboardNavigationEnabled READ keyboardNavigationEnabled WRITE setKeyboardNavigationEnabled NOTIFY keyboardNavigationEnabledChanged)
    Q_PROPERTY(bool screenReaderEnabled READ screenReaderEnabled NOTIFY screenReaderEnabledChanged)

public:
    explicit Accessibility(QObject* parent = nullptr);
    ~Accessibility();

    bool wcagCompliant() const { return m_wcagCompliant; }
    bool keyboardNavigationEnabled() const { return m_keyboardNavigationEnabled; }
    void setKeyboardNavigationEnabled(bool enabled);
    bool screenReaderEnabled() const { return m_screenReaderEnabled; }

    Q_INVOKABLE bool checkColorContrast(const QColor& foreground, const QColor& background);
    Q_INVOKABLE double getContrastRatio(const QColor& foreground, const QColor& background);
    Q_INVOKABLE bool isColorBlindFriendly(const QColor& color1, const QColor& color2);
    Q_INVOKABLE void verifyWCAGCompliance();
    Q_INVOKABLE void detectScreenReader();

signals:
    void wcagCompliantChanged();
    void keyboardNavigationEnabledChanged();
    void screenReaderEnabledChanged();
    void complianceIssueDetected(const QString& issue);

private:
    bool m_wcagCompliant;
    bool m_keyboardNavigationEnabled;
    bool m_screenReaderEnabled;
    
    double calculateLuminance(const QColor& color);
    bool meetsWCAGAA(const QColor& foreground, const QColor& background);
};

#endif // ACCESSIBILITY_H


