#ifndef STATUSINDICATORS_H
#define STATUSINDICATORS_H

#include <QObject>
#include <QString>

/**
 * @brief Status indicators for emergency widget
 * 
 * Provides network status and secure mode status indicators
 */
class StatusIndicators : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool networkActive READ networkActive NOTIFY networkActiveChanged)
    Q_PROPERTY(bool secureModeActive READ secureModeActive NOTIFY secureModeActiveChanged)
    Q_PROPERTY(QString networkStatus READ networkStatus NOTIFY networkStatusChanged)
    Q_PROPERTY(QString secureModeStatus READ secureModeStatus NOTIFY secureModeStatusChanged)

public:
    explicit StatusIndicators(QObject* parent = nullptr);
    ~StatusIndicators();

    bool networkActive() const { return m_networkActive; }
    bool secureModeActive() const { return m_secureModeActive; }
    QString networkStatus() const { return m_networkStatus; }
    QString secureModeStatus() const { return m_secureModeStatus; }

    Q_INVOKABLE void updateNetworkStatus();
    Q_INVOKABLE void updateSecureModeStatus();
    Q_INVOKABLE void updateAll();

signals:
    void networkActiveChanged();
    void secureModeActiveChanged();
    void networkStatusChanged();
    void secureModeStatusChanged();

private:
    bool m_networkActive;
    bool m_secureModeActive;
    QString m_networkStatus;
    QString m_secureModeStatus;
    
    bool checkNetworkStatus();
    bool checkSecureModeStatus();
};

#endif // STATUSINDICATORS_H

