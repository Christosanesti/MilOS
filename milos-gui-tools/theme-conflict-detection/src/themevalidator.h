#ifndef THEMEVALIDATOR_H
#define THEMEVALIDATOR_H

#include <QObject>
#include <QVariantMap>
#include <QStringList>
#include "conflictdetectionrules.h"
#include "compatibilitydatabase.h"

/**
 * @brief Theme Validator
 * 
 * Validates theme compatibility with MilOS xenon effects
 */
class ThemeValidator : public QObject
{
    Q_OBJECT

public:
    explicit ThemeValidator(QObject* parent = nullptr);

    struct ValidationResult {
        CompatibilityDatabase::CompatibilityStatus status;
        QList<ConflictDetectionRules::Conflict> conflicts;
        QStringList errors;
        QStringList warnings;
    };

    /**
     * @brief Validate theme compatibility
     * @param themeId Theme identifier
     * @param themeInfo Theme information
     * @return Validation result
     */
    ValidationResult validateTheme(const QString& themeId, const QVariantMap& themeInfo);

    /**
     * @brief Get theme information from KDE Plasma
     * @param themeId Theme identifier
     * @return Theme information map
     */
    QVariantMap getThemeInfo(const QString& themeId);

signals:
    void themeValidated(const QString& themeId, CompatibilityDatabase::CompatibilityStatus status);

private:
    ConflictDetectionRules* m_detectionRules;
    CompatibilityDatabase* m_compatibilityDatabase;
};

#endif // THEMEVALIDATOR_H

