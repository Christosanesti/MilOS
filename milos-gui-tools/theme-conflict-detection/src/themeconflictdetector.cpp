#include "themeconflictdetector.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QTimer>

ThemeConflictDetector::ThemeConflictDetector(QObject* parent)
    : QObject(parent)
    , m_validator(new ThemeValidator(this))
    , m_compatibilityDatabase(new CompatibilityDatabase(this))
    , m_detectionRules(new ConflictDetectionRules(this))
    , m_notifier(new ConflictNotifier(this))
    , m_running(false)
    , m_initialized(false)
    , m_currentThemeId("")
{
}

ThemeConflictDetector::~ThemeConflictDetector()
{
    stop();
}

bool ThemeConflictDetector::initialize()
{
    if (m_initialized) {
        return true;
    }

    m_initialized = true;
    return true;
}

bool ThemeConflictDetector::start()
{
    if (!m_initialized) {
        qWarning() << "Theme Conflict Detector not initialized";
        return false;
    }

    if (m_running) {
        return true;
    }

    if (!registerInterface()) {
        qWarning() << "Failed to register D-Bus interface";
        return false;
    }

    // Start monitoring theme changes
    monitorThemeChanges();

    // Validate current theme
    validateCurrentTheme();

    m_running = true;
    qDebug() << "Theme Conflict Detector service started";
    return true;
}

void ThemeConflictDetector::stop()
{
    if (!m_running) {
        return;
    }

    unregisterInterface();
    m_running = false;
    qDebug() << "Theme Conflict Detector service stopped";
}

bool ThemeConflictDetector::registerInterface()
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    
    if (!connection.isConnected()) {
        qWarning() << "Cannot connect to D-Bus session bus";
        return false;
    }

    // Register service
    if (!connection.registerService("org.milos.ThemeConflictDetector")) {
        QDBusError error = connection.lastError();
        if (error.isValid()) {
            qWarning() << "Failed to register D-Bus service:" << error.message();
        }
        return false;
    }

    // Register object
    if (!connection.registerObject("/org/milos/ThemeConflictDetector", this,
                                   QDBusConnection::ExportAllSlots |
                                   QDBusConnection::ExportAllSignals)) {
        QDBusError error = connection.lastError();
        if (error.isValid()) {
            qWarning() << "Failed to register D-Bus object:" << error.message();
            connection.unregisterService("org.milos.ThemeConflictDetector");
            return false;
        }
    }

    return true;
}

void ThemeConflictDetector::unregisterInterface()
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    connection.unregisterObject("/org/milos/ThemeConflictDetector");
    connection.unregisterService("org.milos.ThemeConflictDetector");
}

void ThemeConflictDetector::monitorThemeChanges()
{
    // Connect to KDE Plasma theme change signals
    QDBusConnection connection = QDBusConnection::sessionBus();
    
    // Connect to theme change signal from KDE Plasma
    // Note: Actual signal name may differ
    connection.connect("org.kde.KWin",
                      "/KWin",
                      "org.kde.KWin",
                      "themeChanged",
                      this,
                      SLOT(onThemeChanged()));

    // Also monitor for theme installation
    connection.connect("org.kde.KWin",
                      "/KWin",
                      "org.kde.KWin",
                      "themeInstalled",
                      this,
                      SLOT(onThemeInstalled(QString)));
}

void ThemeConflictDetector::validateCurrentTheme()
{
    // Get current theme ID
    QDBusConnection connection = QDBusConnection::sessionBus();
    QDBusInterface themeInterface("org.kde.KWin", "/KWin", "org.kde.KWin", connection);

    if (!themeInterface.isValid()) {
        return;
    }

    QDBusMessage message = QDBusMessage::createMethodCall(
        "org.kde.KWin",
        "/KWin",
        "org.kde.KWin",
        "getTheme"
    );

    QDBusReply<QString> reply = connection.call(message);
    if (reply.isValid()) {
        m_currentThemeId = reply.value();
        ValidateTheme(m_currentThemeId);
    }
}

void ThemeConflictDetector::onThemeChanged()
{
    qDebug() << "Theme changed detected";
    validateCurrentTheme();
}

void ThemeConflictDetector::onThemeInstalled(const QString& themeId)
{
    qDebug() << "Theme installed:" << themeId;
    // Validate newly installed theme
    ValidateTheme(themeId);
}

QVariantMap ThemeConflictDetector::ValidateTheme(const QString& themeId)
{
    ThemeValidator::ValidationResult result = m_validator->validateTheme(themeId, QVariantMap());

    QVariantMap response;
    response["theme_id"] = themeId;
    
    QString statusStr = "unknown";
    switch (result.status) {
        case CompatibilityDatabase::Compatible:
            statusStr = "compatible";
            break;
        case CompatibilityDatabase::Incompatible:
            statusStr = "incompatible";
            break;
        default:
            statusStr = "unknown";
    }
    response["status"] = statusStr;

    // Convert conflicts to variant list
    QVariantList conflictsList;
    for (const ConflictDetectionRules::Conflict& conflict : result.conflicts) {
        QVariantMap conflictMap;
        conflictMap["type"] = static_cast<int>(conflict.type);
        conflictMap["severity"] = static_cast<int>(conflict.severity);
        conflictMap["description"] = conflict.description;
        conflictMap["impact"] = conflict.impact;
        conflictsList.append(conflictMap);
    }
    response["conflicts"] = conflictsList;
    response["errors"] = result.errors;
    response["warnings"] = result.warnings;

    emit ThemeValidated(themeId, statusStr);

    // Emit conflict signal if conflicts detected
    if (!result.conflicts.isEmpty()) {
        emit ThemeConflictDetected(themeId, conflictsList);
        
        // Show notification
        ConflictDetectionRules::SeverityLevel maxSeverity = ConflictDetectionRules::Low;
        for (const ConflictDetectionRules::Conflict& conflict : result.conflicts) {
            if (conflict.severity > maxSeverity) {
                maxSeverity = conflict.severity;
            }
        }
        m_notifier->showConflictNotification(themeId, conflictsList, maxSeverity);
    }

    return response;
}

QVariantList ThemeConflictDetector::GetThemeConflicts(const QString& themeId)
{
    ThemeValidator::ValidationResult result = m_validator->validateTheme(themeId, QVariantMap());

    QVariantList conflictsList;
    for (const ConflictDetectionRules::Conflict& conflict : result.conflicts) {
        QVariantMap conflictMap;
        conflictMap["type"] = static_cast<int>(conflict.type);
        conflictMap["severity"] = static_cast<int>(conflict.severity);
        conflictMap["description"] = conflict.description;
        conflictMap["impact"] = conflict.impact;
        conflictsList.append(conflictMap);
    }

    return conflictsList;
}

QString ThemeConflictDetector::GetCompatibilityStatus(const QString& themeId)
{
    CompatibilityDatabase::CompatibilityStatus status = m_compatibilityDatabase->getThemeStatus(themeId);
    
    switch (status) {
        case CompatibilityDatabase::Compatible:
            return "compatible";
        case CompatibilityDatabase::Incompatible:
            return "incompatible";
        default:
            return "unknown";
    }
}

bool ThemeConflictDetector::UpdateCompatibilityDatabase(const QString& themeId, const QString& status)
{
    CompatibilityDatabase::CompatibilityStatus compatStatus = CompatibilityDatabase::Unknown;
    
    if (status == "compatible") {
        compatStatus = CompatibilityDatabase::Compatible;
    } else if (status == "incompatible") {
        compatStatus = CompatibilityDatabase::Incompatible;
    }

    m_compatibilityDatabase->updateThemeStatus(themeId, compatStatus, QVariantMap());
    return true;
}

