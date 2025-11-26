#ifndef PERFORMANCEMODE_H
#define PERFORMANCEMODE_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include "performancemonitor.h"

/**
 * @brief Performance Mode Manager
 * 
 * Manages performance modes and automatic switching based on degradation
 */
class PerformanceMode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentMode READ currentMode NOTIFY modeChanged)

public:
    enum ModeType {
        Standard,
        Performance,
        LowPower
    };
    Q_ENUM(ModeType)

    explicit PerformanceMode(QObject* parent = nullptr);
    ~PerformanceMode();

    QString currentMode() const { return m_currentMode; }

    Q_INVOKABLE void setMode(const QString& mode);
    Q_INVOKABLE void enableAutoSwitch(bool enabled);
    Q_INVOKABLE QVariantMap getModeConfig() const;

signals:
    void modeChanged(const QString& mode);
    void modeSwitched(const QString& mode);

private slots:
    void onPerformanceDegraded();
    void onPerformanceRecovered();

private:
    void applyMode(const QString& mode);
    void adjustXenonEffects(const QString& mode);

    QString m_currentMode;
    bool m_autoSwitchEnabled;
    PerformanceMonitor* m_monitor;
};

#endif // PERFORMANCEMODE_H


