#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H

#include <QObject>
#include <QString>

/**
 * @brief Notification Manager for Fallback Styling
 * 
 * Manages notifications when fallback styling activates
 */
class NotificationManager : public QObject
{
    Q_OBJECT

public:
    explicit NotificationManager(QObject *parent = nullptr);
    
    Q_INVOKABLE void showFallbackNotification(const QString &reason);
    Q_INVOKABLE void showCompatibilityNotification(const QString &themeName, const QString &status);

signals:
    void notificationRequested(const QString &title, const QString &message, const QString &type);

private:
    QString getFixInstructions(const QString &reason);
    QStringList getCompatibleThemes();
};

#endif // NOTIFICATION_MANAGER_H

