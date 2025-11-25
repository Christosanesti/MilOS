#ifndef TEMPLATEVALIDATOR_H
#define TEMPLATEVALIDATOR_H

#include <QObject>
#include <QVariantMap>
#include <QStringList>

/**
 * @brief Template Validator
 * 
 * Validates layout templates against monitor configurations
 */
class TemplateValidator : public QObject
{
    Q_OBJECT

public:
    explicit TemplateValidator(QObject* parent = nullptr);

    struct ValidationResult {
        bool valid;
        QStringList errors;
        QStringList warnings;
    };

    ValidationResult validateTemplate(const QVariantMap& template, const QVariantMap& monitorConfig);
    bool validateMonitorCount(const QVariantMap& template, const QVariantMap& monitorConfig);
    bool validateResolutions(const QVariantMap& template, const QVariantMap& monitorConfig);
    bool validateWindowPositions(const QVariantMap& template, const QVariantMap& monitorConfig);

private:
    QStringList checkMonitorCount(const QVariantMap& template, const QVariantMap& monitorConfig);
    QStringList checkResolutions(const QVariantMap& template, const QVariantMap& monitorConfig);
    QStringList checkWindowPositions(const QVariantMap& template, const QVariantMap& monitorConfig);
};

#endif // TEMPLATEVALIDATOR_H


