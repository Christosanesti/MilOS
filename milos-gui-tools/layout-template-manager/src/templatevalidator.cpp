#include "templatevalidator.h"
#include <QDebug>

TemplateValidator::TemplateValidator(QObject* parent)
    : QObject(parent)
{
}

TemplateValidator::ValidationResult TemplateValidator::validateTemplate(
    const QVariantMap& template, const QVariantMap& monitorConfig)
{
    ValidationResult result;
    result.valid = true;
    result.errors.clear();
    result.warnings.clear();

    // Check monitor count
    QStringList monitorErrors = checkMonitorCount(template, monitorConfig);
    result.errors.append(monitorErrors);

    // Check resolutions
    QStringList resolutionErrors = checkResolutions(template, monitorConfig);
    result.errors.append(resolutionErrors);

    // Check window positions
    QStringList positionWarnings = checkWindowPositions(template, monitorConfig);
    result.warnings.append(positionWarnings);

    result.valid = result.errors.isEmpty();
    return result;
}

bool TemplateValidator::validateMonitorCount(const QVariantMap& template, const QVariantMap& monitorConfig)
{
    int requiredMonitors = template.value("monitor_count", template.value("monitors", 1)).toInt();
    int availableMonitors = monitorConfig.value("count", 1).toInt();

    return availableMonitors >= requiredMonitors;
}

bool TemplateValidator::validateResolutions(const QVariantMap& template, const QVariantMap& monitorConfig)
{
    // Check if monitor configurations match
    if (!template.contains("monitor_configurations")) {
        return true; // No specific resolution requirements
    }

    QVariantList templateMonitors = template.value("monitor_configurations").toList();
    QVariantList availableMonitors = monitorConfig.value("monitors", QVariantList()).toList();

    if (availableMonitors.isEmpty()) {
        return true; // Cannot validate without monitor info
    }

    // Check each template monitor has matching available monitor
    for (const QVariant& templateMonitorVar : templateMonitors) {
        QVariantMap templateMonitor = templateMonitorVar.toMap();
        int index = templateMonitor.value("index", -1).toInt();
        
        if (index < 0 || index >= availableMonitors.size()) {
            continue; // Index out of range, will be caught by monitor count check
        }

        QVariantMap availableMonitor = availableMonitors.at(index).toMap();
        QVariantMap templateRes = templateMonitor.value("resolution").toMap();
        QVariantMap availableRes = availableMonitor.value("resolution", availableMonitor).toMap();

        int templateWidth = templateRes.value("width", 0).toInt();
        int templateHeight = templateRes.value("height", 0).toInt();
        int availableWidth = availableRes.value("width", 0).toInt();
        int availableHeight = availableRes.value("height", 0).toInt();

        if (templateWidth > 0 && templateHeight > 0) {
            if (availableWidth < templateWidth || availableHeight < templateHeight) {
                return false;
            }
        }
    }

    return true;
}

bool TemplateValidator::validateWindowPositions(const QVariantMap& template, const QVariantMap& monitorConfig)
{
    if (!template.contains("window_positions")) {
        return true;
    }

    QVariantList windowPositions = template.value("window_positions").toList();
    QVariantList availableMonitors = monitorConfig.value("monitors", QVariantList()).toList();

    for (const QVariant& windowVar : windowPositions) {
        QVariantMap window = windowVar.toMap();
        int monitorIndex = window.value("monitor_index", 0).toInt();

        if (monitorIndex >= availableMonitors.size()) {
            return false; // Window references non-existent monitor
        }
    }

    return true;
}

QStringList TemplateValidator::checkMonitorCount(const QVariantMap& template, const QVariantMap& monitorConfig)
{
    QStringList errors;

    int requiredMonitors = template.value("monitor_count", template.value("monitors", 1)).toInt();
    int availableMonitors = monitorConfig.value("count", 1).toInt();

    if (availableMonitors < requiredMonitors) {
        errors << QString("Template requires %1 monitor(s), but only %2 available")
                  .arg(requiredMonitors).arg(availableMonitors);
    }

    return errors;
}

QStringList TemplateValidator::checkResolutions(const QVariantMap& template, const QVariantMap& monitorConfig)
{
    QStringList errors;

    if (!template.contains("monitor_configurations")) {
        return errors; // No resolution requirements
    }

    QVariantList templateMonitors = template.value("monitor_configurations").toList();
    QVariantList availableMonitors = monitorConfig.value("monitors", QVariantList()).toList();

    if (availableMonitors.isEmpty()) {
        return errors; // Cannot check without monitor info
    }

    for (int i = 0; i < templateMonitors.size(); ++i) {
        QVariantMap templateMonitor = templateMonitors.at(i).toMap();
        QVariantMap templateRes = templateMonitor.value("resolution").toMap();

        int templateWidth = templateRes.value("width", 0).toInt();
        int templateHeight = templateRes.value("height", 0).toInt();

        if (templateWidth > 0 && templateHeight > 0) {
            if (i < availableMonitors.size()) {
                QVariantMap availableMonitor = availableMonitors.at(i).toMap();
                QVariantMap availableRes = availableMonitor.value("resolution", availableMonitor).toMap();
                int availableWidth = availableRes.value("width", 0).toInt();
                int availableHeight = availableRes.value("height", 0).toInt();

                if (availableWidth > 0 && availableHeight > 0) {
                    if (availableWidth < templateWidth || availableHeight < templateHeight) {
                        errors << QString("Monitor %1: Template requires %2x%3, but monitor is %4x%5")
                                  .arg(i).arg(templateWidth).arg(templateHeight)
                                  .arg(availableWidth).arg(availableHeight);
                    }
                }
            }
        }
    }

    return errors;
}

QStringList TemplateValidator::checkWindowPositions(const QVariantMap& template, const QVariantMap& monitorConfig)
{
    QStringList warnings;

    if (!template.contains("window_positions")) {
        return warnings;
    }

    QVariantList windowPositions = template.value("window_positions").toList();
    int availableMonitors = monitorConfig.value("count", 1).toInt();

    for (const QVariant& windowVar : windowPositions) {
        QVariantMap window = windowVar.toMap();
        int monitorIndex = window.value("monitor_index", 0).toInt();

        if (monitorIndex >= availableMonitors) {
            warnings << QString("Window '%1' references monitor %2, but only %3 monitor(s) available")
                        .arg(window.value("application_name").toString())
                        .arg(monitorIndex)
                        .arg(availableMonitors);
        }
    }

    return warnings;
}


