#ifndef FALLBACKSTYLING_H
#define FALLBACKSTYLING_H

#include <QObject>
#include <QString>
#include <QColor>

class NotificationManager;

/**
 * @brief Fallback Navigation Styling
 * 
 * Provides fallback high-contrast styling when xenon effects cannot render
 */
class FallbackStyling : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
    Q_PROPERTY(QString reason READ reason NOTIFY reasonChanged)

public:
    explicit FallbackStyling(QObject* parent = nullptr);
    ~FallbackStyling();

    bool active() const { return m_active; }
    QString reason() const { return m_reason; }

    Q_INVOKABLE void checkXenonEffects();
    Q_INVOKABLE QColor getFallbackColor(const QString& componentType);
    Q_INVOKABLE int getFallbackBorderWidth(const QString& componentType);
    Q_INVOKABLE int getFallbackFontSize(const QString& componentType);

signals:
    void activeChanged();
    void reasonChanged();
    void fallbackActivated(const QString& reason);

private:
    bool m_active;
    QString m_reason;
    NotificationManager* m_notificationManager;
    
    bool canRenderXenonEffects();
    void activateFallback(const QString& reason);
    void deactivateFallback();
};

#endif // FALLBACKSTYLING_H

