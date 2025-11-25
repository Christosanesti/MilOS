#ifndef CONFLICTDETECTIONRULES_H
#define CONFLICTDETECTIONRULES_H

#include <QObject>
#include <QVariantMap>
#include <QStringList>

/**
 * @brief Conflict Detection Rules
 * 
 * Defines rules for detecting theme conflicts with MilOS xenon effects
 */
class ConflictDetectionRules : public QObject
{
    Q_OBJECT

public:
    enum ConflictType {
        XenonRendering,
        ColorScheme,
        ComponentStyle,
        Performance
    };
    Q_ENUM(ConflictType)

    enum SeverityLevel {
        Low,
        Medium,
        High,
        Critical
    };
    Q_ENUM(SeverityLevel)

    explicit ConflictDetectionRules(QObject* parent = nullptr);

    struct Conflict {
        ConflictType type;
        SeverityLevel severity;
        QString description;
        QString impact;
    };

    /**
     * @brief Detect conflicts for a theme
     * @param themeInfo Theme information
     * @return List of detected conflicts
     */
    QList<Conflict> detectConflicts(const QVariantMap& themeInfo);

    /**
     * @brief Test xenon effect rendering
     * @return true if rendering works, false if conflicts detected
     */
    bool testXenonRendering();

    /**
     * @brief Check color scheme conflicts
     * @param themeInfo Theme information
     * @return List of color scheme conflicts
     */
    QList<Conflict> checkColorSchemeConflicts(const QVariantMap& themeInfo);

    /**
     * @brief Check component style conflicts
     * @param themeInfo Theme information
     * @return List of component style conflicts
     */
    QList<Conflict> checkComponentStyleConflicts(const QVariantMap& themeInfo);

    /**
     * @brief Check performance conflicts
     * @param themeInfo Theme information
     * @return List of performance conflicts
     */
    QList<Conflict> checkPerformanceConflicts(const QVariantMap& themeInfo);

    /**
     * @brief Assign severity level based on conflict type
     * @param conflictType Conflict type
     * @return Severity level
     */
    SeverityLevel assignSeverity(ConflictType conflictType);

signals:
    void conflictDetected(ConflictType type, SeverityLevel severity);

private:
    bool testXenonEffect(const QString& effectName);
};

#endif // CONFLICTDETECTIONRULES_H

