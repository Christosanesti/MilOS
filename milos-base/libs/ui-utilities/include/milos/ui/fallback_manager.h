#ifndef MILOS_UI_FALLBACK_MANAGER_H
#define MILOS_UI_FALLBACK_MANAGER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QHash>

/**
 * @brief Fallback Manager
 * 
 * Manages graceful degradation when features are unavailable.
 * Provides fallback UI, styling, and functionality.
 */
class FallbackManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool xenonEffectsAvailable READ xenonEffectsAvailable NOTIFY xenonEffectsChanged)
    Q_PROPERTY(bool hardwareAccelerationAvailable READ hardwareAccelerationAvailable NOTIFY hardwareAccelerationChanged)
    Q_PROPERTY(bool networkAvailable READ networkAvailable NOTIFY networkChanged)
    Q_PROPERTY(bool serviceAvailable READ serviceAvailable NOTIFY serviceChanged)

public:
    explicit FallbackManager(QObject* parent = nullptr);
    ~FallbackManager();

    /**
     * @brief Get singleton instance
     */
    static FallbackManager* instance();

    /**
     * @brief Initialize fallback manager
     * @return true if successful
     */
    bool initialize();

    /**
     * @brief Check feature availability
     */
    bool isFeatureAvailable(const QString& featureName) const;

    /**
     * @brief Get fallback value for feature
     */
    QVariant getFallbackValue(const QString& featureName, const QVariant& defaultValue) const;

    /**
     * @brief Register fallback handler
     */
    void registerFallback(const QString& featureName, const QVariant& fallbackValue);

    // Property getters
    bool xenonEffectsAvailable() const { return m_xenonEffectsAvailable; }
    bool hardwareAccelerationAvailable() const { return m_hardwareAccelerationAvailable; }
    bool networkAvailable() const { return m_networkAvailable; }
    bool serviceAvailable() const { return m_serviceAvailable; }

signals:
    void xenonEffectsChanged();
    void hardwareAccelerationChanged();
    void networkChanged();
    void serviceChanged();
    void featureUnavailable(const QString& featureName, const QString& reason);

private:
    void checkXenonEffects();
    void checkHardwareAcceleration();
    void checkNetwork();
    void checkServices();

    static FallbackManager* s_instance;
    bool m_xenonEffectsAvailable;
    bool m_hardwareAccelerationAvailable;
    bool m_networkAvailable;
    bool m_serviceAvailable;
    QHash<QString, QVariant> m_fallbacks;
};

#endif // MILOS_UI_FALLBACK_MANAGER_H



#define MILOS_UI_FALLBACK_MANAGER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QHash>

/**
 * @brief Fallback Manager
 * 
 * Manages graceful degradation when features are unavailable.
 * Provides fallback UI, styling, and functionality.
 */
class FallbackManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool xenonEffectsAvailable READ xenonEffectsAvailable NOTIFY xenonEffectsChanged)
    Q_PROPERTY(bool hardwareAccelerationAvailable READ hardwareAccelerationAvailable NOTIFY hardwareAccelerationChanged)
    Q_PROPERTY(bool networkAvailable READ networkAvailable NOTIFY networkChanged)
    Q_PROPERTY(bool serviceAvailable READ serviceAvailable NOTIFY serviceChanged)

public:
    explicit FallbackManager(QObject* parent = nullptr);
    ~FallbackManager();

    /**
     * @brief Get singleton instance
     */
    static FallbackManager* instance();

    /**
     * @brief Initialize fallback manager
     * @return true if successful
     */
    bool initialize();

    /**
     * @brief Check feature availability
     */
    bool isFeatureAvailable(const QString& featureName) const;

    /**
     * @brief Get fallback value for feature
     */
    QVariant getFallbackValue(const QString& featureName, const QVariant& defaultValue) const;

    /**
     * @brief Register fallback handler
     */
    void registerFallback(const QString& featureName, const QVariant& fallbackValue);

    // Property getters
    bool xenonEffectsAvailable() const { return m_xenonEffectsAvailable; }
    bool hardwareAccelerationAvailable() const { return m_hardwareAccelerationAvailable; }
    bool networkAvailable() const { return m_networkAvailable; }
    bool serviceAvailable() const { return m_serviceAvailable; }

signals:
    void xenonEffectsChanged();
    void hardwareAccelerationChanged();
    void networkChanged();
    void serviceChanged();
    void featureUnavailable(const QString& featureName, const QString& reason);

private:
    void checkXenonEffects();
    void checkHardwareAcceleration();
    void checkNetwork();
    void checkServices();

    static FallbackManager* s_instance;
    bool m_xenonEffectsAvailable;
    bool m_hardwareAccelerationAvailable;
    bool m_networkAvailable;
    bool m_serviceAvailable;
    QHash<QString, QVariant> m_fallbacks;
};

#endif // MILOS_UI_FALLBACK_MANAGER_H


