#ifndef QUICKACTIONSSERVICE_H
#define QUICKACTIONSSERVICE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QTimer>
#include <QDBusConnection>
#include <QDBusInterface>

/**
 * @brief Service for managing contextual quick actions
 * 
 * Provides interface to quick actions D-Bus service for context detection,
 * action availability, and action execution.
 */
class QuickActionsService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentContext READ currentContext NOTIFY currentContextChanged)
    Q_PROPERTY(QVariantList availableActions READ availableActions NOTIFY availableActionsChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

public:
    explicit QuickActionsService(QObject *parent = nullptr);
    ~QuickActionsService();

    QString currentContext() const { return m_currentContext; }
    QVariantList availableActions() const { return m_availableActions; }
    bool enabled() const { return m_enabled; }
    void setEnabled(bool enabled);

    Q_INVOKABLE void executeAction(const QString &actionId);
    Q_INVOKABLE void refreshContext();
    Q_INVOKABLE QVariantMap getActionInfo(const QString &actionId);

signals:
    void currentContextChanged(const QString &context);
    void availableActionsChanged();
    void enabledChanged(bool enabled);
    void actionExecuted(const QString &actionId, bool success);
    void actionError(const QString &errorMessage);

private slots:
    void updateContext();
    void onActiveWindowChanged();

private:
    QString detectApplicationContext();
    void updateAvailableActions();
    bool callToolDBusMethod(const QString &service, const QString &path, 
                           const QString &interface, const QString &method,
                           const QVariantList &args = QVariantList());

    QString m_currentContext;
    QVariantList m_availableActions;
    bool m_enabled;
    QTimer *m_contextTimer;
    QDBusConnection m_dbusConnection;
    QDBusInterface *m_quickActionsInterface;
};

#endif // QUICKACTIONSSERVICE_H

