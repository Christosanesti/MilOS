#ifndef ALERTZONESERVICE_H
#define ALERTZONESERVICE_H

#include <QObject>
#include <QString>

/**
 * @brief Service for managing alert zone alerts
 * 
 * Provides D-Bus interface for receiving and managing alerts
 * in the critical alert zone widget.
 */
class AlertZoneService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int alertCount READ alertCount NOTIFY alertCountChanged)

public:
    explicit AlertZoneService(QObject* parent = nullptr);
    ~AlertZoneService();

    int alertCount() const { return m_alertCount; }

public Q_SLOTS:
    Q_INVOKABLE void addAlert(const QString& severity, const QString& category, const QString& message);
    Q_INVOKABLE void clearAlerts();

Q_SIGNALS:
    void alertReceived(const QString& severity, const QString& category, const QString& message);
    void alertCountChanged(int count);
    void alertsCleared();

private:
    int m_alertCount;
};

#endif // ALERTZONESERVICE_H

