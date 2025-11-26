#ifndef ACTIONDETECTOR_H
#define ACTIONDETECTOR_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QVariantList>

/**
 * @brief Action Detector
 * 
 * Detects active window and available actions based on application context
 */
class ActionDetector : public QObject
{
    Q_OBJECT

public:
    explicit ActionDetector(QObject* parent = nullptr);

    /**
     * @brief Get active window context
     * @return Active window context map
     */
    QVariantMap getActiveWindowContext();

    /**
     * @brief Get available actions for active window
     * @return List of available actions
     */
    QVariantList getAvailableActions(const QVariantMap& windowContext);

signals:
    void activeWindowChanged(const QString& windowId, const QString& applicationName);

private slots:
    void onActiveWindowChanged();

private:
    QString detectApplicationName(const QString& windowId);
    QVariantList getActionsForApplication(const QString& applicationName);

    QString m_currentWindowId;
    QString m_currentApplicationName;
};

#endif // ACTIONDETECTOR_H


