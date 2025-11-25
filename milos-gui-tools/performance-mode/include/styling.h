#ifndef PERFORMANCE_STYLING_H
#define PERFORMANCE_STYLING_H

#include <QObject>
#include <QString>
#include <QVariantMap>

/**
 * @brief Performance Mode Styling Manager
 * 
 * Manages styling for different performance modes:
 * - Standard: Full xenon effects, all animations, particle effects
 * - Performance: Reduced xenon effects, simplified animations, no particles
 * - Low-Power: Minimal xenon effects, minimal animations, no particles
 */
class PerformanceStyling : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentMode READ currentMode WRITE setCurrentMode NOTIFY currentModeChanged)
    Q_PROPERTY(QVariantMap styleProperties READ styleProperties NOTIFY stylePropertiesChanged)

public:
    enum PerformanceMode {
        Standard = 0,
        Performance = 1,
        LowPower = 2
    };
    Q_ENUM(PerformanceMode)

    explicit PerformanceStyling(QObject *parent = nullptr);
    
    QString currentMode() const { return m_currentMode; }
    void setCurrentMode(const QString &mode);
    
    QVariantMap styleProperties() const { return m_styleProperties; }
    
    Q_INVOKABLE void applyMode(const QString &mode);
    Q_INVOKABLE QVariantMap getModeProperties(const QString &mode);
    Q_INVOKABLE double getXenonIntensity(const QString &mode);
    Q_INVOKABLE double getGlowRadius(const QString &mode);
    Q_INVOKABLE bool areParticlesEnabled(const QString &mode);
    Q_INVOKABLE bool areAnimationsSimplified(const QString &mode);

signals:
    void currentModeChanged();
    void stylePropertiesChanged();

private:
    void updateStyleProperties();
    QVariantMap getStandardModeProperties();
    QVariantMap getPerformanceModeProperties();
    QVariantMap getLowPowerModeProperties();
    
    QString m_currentMode;
    QVariantMap m_styleProperties;
};

#endif // PERFORMANCE_STYLING_H

