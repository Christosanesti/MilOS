#include "conflictdetectionrules.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>

ConflictDetectionRules::ConflictDetectionRules(QObject* parent)
    : QObject(parent)
{
}

QList<ConflictDetectionRules::Conflict> ConflictDetectionRules::detectConflicts(const QVariantMap& themeInfo)
{
    QList<Conflict> conflicts;

    // Test xenon rendering
    if (!testXenonRendering()) {
        Conflict conflict;
        conflict.type = XenonRendering;
        conflict.severity = assignSeverity(XenonRendering);
        conflict.description = "Xenon effects rendering conflicts detected";
        conflict.impact = "Xenon visual effects may not render correctly with this theme";
        conflicts.append(conflict);
        emit conflictDetected(XenonRendering, conflict.severity);
    }

    // Check color scheme conflicts
    conflicts.append(checkColorSchemeConflicts(themeInfo));

    // Check component style conflicts
    conflicts.append(checkComponentStyleConflicts(themeInfo));

    // Check performance conflicts
    conflicts.append(checkPerformanceConflicts(themeInfo));

    return conflicts;
}

bool ConflictDetectionRules::testXenonRendering()
{
    // Test xenon effects via D-Bus interface
    QDBusConnection connection = QDBusConnection::sessionBus();
    QDBusInterface xenonInterface("org.milos.XenonEffects", "/XenonEffects", "org.milos.XenonEffects", connection);

    if (!xenonInterface.isValid()) {
        // Xenon effects service not available - assume compatible
        return true;
    }

    // Test each xenon effect type
    QStringList effects = {"glow", "pulse", "scan", "trails", "particles"};
    bool allWorking = true;

    for (const QString& effect : effects) {
        if (!testXenonEffect(effect)) {
            allWorking = false;
            break;
        }
    }

    return allWorking;
}

bool ConflictDetectionRules::testXenonEffect(const QString& effectName)
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    QDBusInterface xenonInterface("org.milos.XenonEffects", "/XenonEffects", "org.milos.XenonEffects", connection);

    if (!xenonInterface.isValid()) {
        return true; // Service not available, assume compatible
    }

    // Test effect rendering
    QDBusMessage message = QDBusMessage::createMethodCall(
        "org.milos.XenonEffects",
        "/XenonEffects",
        "org.milos.XenonEffects",
        "testEffect"
    );
    message << effectName;

    QDBusReply<bool> reply = connection.call(message);
    if (reply.isValid()) {
        return reply.value();
    }

    // If test method not available, assume compatible
    return true;
}

QList<ConflictDetectionRules::Conflict> ConflictDetectionRules::checkColorSchemeConflicts(const QVariantMap& themeInfo)
{
    QList<Conflict> conflicts;

    // Check if theme uses dark colors that might conflict with xenon effects
    QString colorScheme = themeInfo.value("color_scheme", "").toString().toLower();
    
    if (colorScheme.contains("dark") || colorScheme.contains("black")) {
        // Check if contrast is sufficient for xenon effects
        // This is a simplified check - actual implementation would analyze color values
        Conflict conflict;
        conflict.type = ColorScheme;
        conflict.severity = Medium;
        conflict.description = "Dark color scheme may reduce xenon effect visibility";
        conflict.impact = "Xenon effects may be less visible with dark themes";
        conflicts.append(conflict);
        emit conflictDetected(ColorScheme, conflict.severity);
    }

    return conflicts;
}

QList<ConflictDetectionRules::Conflict> ConflictDetectionRules::checkComponentStyleConflicts(const QVariantMap& themeInfo)
{
    QList<Conflict> conflicts;

    // Check if theme uses custom component styles that might conflict
    QString styleName = themeInfo.value("style", "").toString().toLower();
    
    if (styleName.contains("custom") || styleName.contains("override")) {
        Conflict conflict;
        conflict.type = ComponentStyle;
        conflict.severity = Low;
        conflict.description = "Custom component styles may conflict with MilOS components";
        conflict.impact = "Some MilOS UI components may not display correctly";
        conflicts.append(conflict);
        emit conflictDetected(ComponentStyle, conflict.severity);
    }

    return conflicts;
}

QList<ConflictDetectionRules::Conflict> ConflictDetectionRules::checkPerformanceConflicts(const QVariantMap& themeInfo)
{
    QList<Conflict> conflicts;

    // Check if GPU acceleration is disabled
    bool gpuAcceleration = themeInfo.value("gpu_acceleration", true).toBool();
    
    if (!gpuAcceleration) {
        Conflict conflict;
        conflict.type = Performance;
        conflict.severity = High;
        conflict.description = "GPU acceleration disabled - xenon effects may have performance issues";
        conflict.impact = "Xenon effects may be slow or stutter without GPU acceleration";
        conflicts.append(conflict);
        emit conflictDetected(Performance, conflict.severity);
    }

    return conflicts;
}

ConflictDetectionRules::SeverityLevel ConflictDetectionRules::assignSeverity(ConflictType conflictType)
{
    switch (conflictType) {
        case XenonRendering:
            return Critical; // Xenon rendering is critical for MilOS
        case Performance:
            return High; // Performance issues are high severity
        case ColorScheme:
            return Medium; // Color scheme issues are medium severity
        case ComponentStyle:
            return Low; // Component style issues are low severity
        default:
            return Low;
    }
}

