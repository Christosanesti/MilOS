#ifndef LOWLIGHT_H
#define LOWLIGHT_H

#include <QObject>
#include <QColor>
#include <QSettings>

/**
 * @brief Low-Light Optimization Manager
 * 
 * Manages low-light optimizations for better visibility
 */
class LowLight : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(int brightnessLevel READ brightnessLevel WRITE setBrightnessLevel NOTIFY brightnessLevelChanged)

public:
    explicit LowLight(QObject* parent = nullptr);
    ~LowLight();

    bool enabled() const { return m_enabled; }
    void setEnabled(bool enabled);
    
    int brightnessLevel() const { return m_brightnessLevel; }
    void setBrightnessLevel(int level);

    Q_INVOKABLE QColor getOptimizedColor(const QColor& baseColor);
    Q_INVOKABLE int getOptimizedFontSize(int baseSize);
    Q_INVOKABLE void optimizeForLowLight();
    Q_INVOKABLE void savePreferences();

signals:
    void enabledChanged();
    void brightnessLevelChanged();
    void optimizationApplied();

private:
    void loadPreferences();
    QColor adjustColorForLowLight(const QColor& color);

    bool m_enabled;
    int m_brightnessLevel; // 0-100
    QSettings* m_settings;
};

#endif // LOWLIGHT_H

