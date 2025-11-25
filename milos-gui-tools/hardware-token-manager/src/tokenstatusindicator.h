#ifndef TOKENSTATUSINDICATOR_H
#define TOKENSTATUSINDICATOR_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QTimer>

/**
 * @brief Token Status Indicator
 * 
 * Monitors and displays hardware token status
 */
class TokenStatusIndicator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
    Q_PROPERTY(int batteryLevel READ batteryLevel NOTIFY batteryLevelChanged)
    Q_PROPERTY(QString tokenName READ tokenName NOTIFY tokenNameChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)

public:
    explicit TokenStatusIndicator(QObject* parent = nullptr);
    ~TokenStatusIndicator();

    bool connected() const { return m_connected; }
    bool active() const { return m_active; }
    int batteryLevel() const { return m_batteryLevel; }
    QString tokenName() const { return m_tokenName; }
    QString status() const { return m_status; }

    Q_INVOKABLE void updateStatus();
    Q_INVOKABLE void startMonitoring();
    Q_INVOKABLE void stopMonitoring();

signals:
    void connectedChanged();
    void activeChanged();
    void batteryLevelChanged();
    void tokenNameChanged();
    void statusChanged();
    void tokenActivityDetected();
    void lowBatteryWarning(int level);

private slots:
    void onMonitoringTimer();

private:
    void checkTokenStatus();
    void checkTokenActivity();
    void checkBatteryLevel();

    bool m_connected;
    bool m_active;
    int m_batteryLevel;
    QString m_tokenName;
    QString m_status;
    QTimer* m_monitoringTimer;
    QDBusInterface* m_tokenInterface;
};

#endif // TOKENSTATUSINDICATOR_H

