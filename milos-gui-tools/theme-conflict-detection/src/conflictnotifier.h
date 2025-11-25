#ifndef CONFLICTNOTIFIER_H
#define CONFLICTNOTIFIER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QVariantList>
#include <QStringList>
#include "conflictdetectionrules.h"
#include "compatibilitydatabase.h"

/**
 * @brief Conflict Notifier
 * 
 * Displays notifications when theme conflicts are detected
 */
class ConflictNotifier : public QObject
{
    Q_OBJECT

public:
    explicit ConflictNotifier(QObject* parent = nullptr);

    /**
     * @brief Show notification for theme conflict
     * @param themeId Theme identifier
     * @param conflicts List of conflicts
     * @param severity Severity level
     */
    void showConflictNotification(const QString& themeId, const QVariantList& conflicts, 
                                  ConflictDetectionRules::SeverityLevel severity);

    /**
     * @brief Show notification with recommendations
     * @param themeId Theme identifier
     * @param conflicts List of conflicts
     * @param recommendations List of recommendations
     */
    void showNotificationWithRecommendations(const QString& themeId, 
                                             const QVariantList& conflicts,
                                             const QStringList& recommendations);

signals:
    void notificationActionTriggered(const QString& action, const QString& themeId);

private:
    void sendKdeNotification(const QString& title, const QString& message, 
                             const QStringList& actions, int urgency);
    QStringList generateRecommendations(const QVariantList& conflicts);
    bool isCriticalConflict(const QVariantList& conflicts);
};

#endif // CONFLICTNOTIFIER_H

