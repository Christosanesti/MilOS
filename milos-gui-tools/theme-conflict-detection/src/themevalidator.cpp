#include "themevalidator.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>

ThemeValidator::ThemeValidator(QObject* parent)
    : QObject(parent)
    , m_detectionRules(new ConflictDetectionRules(this))
    , m_compatibilityDatabase(new CompatibilityDatabase(this))
{
}

ThemeValidator::ValidationResult ThemeValidator::validateTheme(const QString& themeId, const QVariantMap& themeInfo)
{
    ValidationResult result;
    result.status = CompatibilityDatabase::Unknown;

    // Check compatibility database first
    CompatibilityDatabase::CompatibilityStatus dbStatus = m_compatibilityDatabase->getThemeStatus(themeId);
    if (dbStatus != CompatibilityDatabase::Unknown) {
        result.status = dbStatus;
        // Still run detection to get current conflicts
    }

    // Get theme information if not provided
    QVariantMap theme = themeInfo;
    if (theme.isEmpty()) {
        theme = getThemeInfo(themeId);
    }

    // Detect conflicts
    QList<ConflictDetectionRules::Conflict> conflicts = m_detectionRules->detectConflicts(theme);
    result.conflicts = conflicts;

    // Determine compatibility status based on conflicts
    if (conflicts.isEmpty()) {
        result.status = CompatibilityDatabase::Compatible;
    } else {
        // Check if any critical conflicts
        bool hasCritical = false;
        bool hasHigh = false;
        
        for (const ConflictDetectionRules::Conflict& conflict : conflicts) {
            if (conflict.severity == ConflictDetectionRules::Critical) {
                hasCritical = true;
                break;
            } else if (conflict.severity == ConflictDetectionRules::High) {
                hasHigh = true;
            }
        }

        if (hasCritical || hasHigh) {
            result.status = CompatibilityDatabase::Incompatible;
        } else {
            // Low/medium severity conflicts - still compatible but with warnings
            result.status = CompatibilityDatabase::Compatible;
            for (const ConflictDetectionRules::Conflict& conflict : conflicts) {
                result.warnings.append(conflict.description);
            }
        }
    }

    // Update compatibility database
    QVariantMap conflictMap;
    for (const ConflictDetectionRules::Conflict& conflict : conflicts) {
        conflictMap[QString::number(conflict.type)] = conflict.description;
    }
    m_compatibilityDatabase->updateThemeStatus(themeId, result.status, conflictMap);

    emit themeValidated(themeId, result.status);
    return result;
}

QVariantMap ThemeValidator::getThemeInfo(const QString& themeId)
{
    QVariantMap themeInfo;

    // Get theme information from KDE Plasma theme API
    QDBusConnection connection = QDBusConnection::sessionBus();
    QDBusInterface themeInterface("org.kde.KWin", "/KWin", "org.kde.KWin", connection);

    if (!themeInterface.isValid()) {
        qWarning() << "Cannot connect to KDE Plasma theme API";
        return themeInfo;
    }

    // Get current theme name
    // Note: Actual KDE Plasma API methods may differ
    QDBusMessage message = QDBusMessage::createMethodCall(
        "org.kde.KWin",
        "/KWin",
        "org.kde.KWin",
        "getTheme"
    );

    QDBusReply<QString> reply = connection.call(message);
    if (reply.isValid()) {
        themeInfo["name"] = reply.value();
        themeInfo["id"] = themeId;
    }

    // Get color scheme
    QDBusMessage colorMessage = QDBusMessage::createMethodCall(
        "org.kde.KWin",
        "/KWin",
        "org.kde.KWin",
        "getColorScheme"
    );

    QDBusReply<QString> colorReply = connection.call(colorMessage);
    if (colorReply.isValid()) {
        themeInfo["color_scheme"] = colorReply.value();
    }

    return themeInfo;
}

