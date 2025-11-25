#ifndef HIGHCONTRASTMODE_H
#define HIGHCONTRASTMODE_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QSettings>

/**
 * @brief High-Contrast Mode Manager
 * 
 * Manages high-contrast mode activation, configuration, and persistence
 */
class HighContrastMode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool modeEnabled READ modeEnabled NOTIFY modeEnabledChanged)
    Q_PROPERTY(double xenonIntensityMultiplier READ xenonIntensityMultiplier NOTIFY configChanged)
    Q_PROPERTY(double xenonGlowRadiusMultiplier READ xenonGlowRadiusMultiplier NOTIFY configChanged)

public:
    explicit HighContrastMode(QObject* parent = nullptr);
    ~HighContrastMode();

    bool modeEnabled() const { return m_modeEnabled; }
    double xenonIntensityMultiplier() const { return m_xenonIntensityMultiplier; }
    double xenonGlowRadiusMultiplier() const { return m_xenonGlowRadiusMultiplier; }
    double borderThicknessMultiplier() const { return m_borderThicknessMultiplier; }
    int clickTargetSize() const { return m_clickTargetSize; }

    Q_INVOKABLE void activateMode();
    Q_INVOKABLE void deactivateMode();
    Q_INVOKABLE void toggleMode();
    Q_INVOKABLE QVariantMap getModeConfig() const;
    Q_INVOKABLE void setModeConfig(const QVariantMap& config);

signals:
    void modeEnabledChanged(bool enabled);
    void modeActivated();
    void modeDeactivated();
    void configChanged();

private:
    void loadConfiguration();
    void saveConfiguration();
    void applyTheme();
    void adjustXenonEffects();

    bool m_modeEnabled;
    double m_xenonIntensityMultiplier;
    double m_xenonGlowRadiusMultiplier;
    double m_borderThicknessMultiplier;
    int m_clickTargetSize;
    QSettings* m_settings;
};

#endif // HIGHCONTRASTMODE_H

