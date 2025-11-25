#ifndef PERFORMANCE_CONTROL_H
#define PERFORMANCE_CONTROL_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QStringList>

class PerformanceStyling;

/**
 * @brief Performance Mode Control Manager
 * 
 * Manages manual control of performance modes including:
 * - System-wide mode selection
 * - Per-application mode overrides
 * - Performance mode override (force specific mode)
 */
class PerformanceControl : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentMode READ currentMode WRITE setCurrentMode NOTIFY currentModeChanged)
    Q_PROPERTY(QStringList availableModes READ availableModes CONSTANT)
    Q_PROPERTY(QVariantMap applicationOverrides READ applicationOverrides NOTIFY applicationOverridesChanged)
    Q_PROPERTY(bool overrideEnabled READ overrideEnabled WRITE setOverrideEnabled NOTIFY overrideEnabledChanged)
    Q_PROPERTY(QString overrideMode READ overrideMode WRITE setOverrideMode NOTIFY overrideModeChanged)

public:
    explicit PerformanceControl(QObject *parent = nullptr);
    
    QString currentMode() const { return m_currentMode; }
    void setCurrentMode(const QString &mode);
    
    QStringList availableModes() const { return QStringList() << "standard" << "performance" << "low-power"; }
    
    QVariantMap applicationOverrides() const { return m_applicationOverrides; }
    
    bool overrideEnabled() const { return m_overrideEnabled; }
    void setOverrideEnabled(bool enabled);
    
    QString overrideMode() const { return m_overrideMode; }
    void setOverrideMode(const QString &mode);
    
    Q_INVOKABLE void setApplicationMode(const QString &applicationId, const QString &mode);
    Q_INVOKABLE QString getApplicationMode(const QString &applicationId);
    Q_INVOKABLE void removeApplicationOverride(const QString &applicationId);
    Q_INVOKABLE void clearAllOverrides();
    Q_INVOKABLE void loadConfiguration();
    Q_INVOKABLE void saveConfiguration();

signals:
    void currentModeChanged();
    void applicationOverridesChanged();
    void overrideEnabledChanged();
    void overrideModeChanged();
    void modeApplied(const QString &mode);

private:
    QString m_currentMode;
    QVariantMap m_applicationOverrides;
    bool m_overrideEnabled;
    QString m_overrideMode;
    QString m_configPath;
    
    PerformanceStyling* m_styling;
};

#endif // PERFORMANCE_CONTROL_H

