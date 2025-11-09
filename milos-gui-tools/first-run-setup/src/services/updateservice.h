#ifndef UPDATESERVICE_H
#define UPDATESERVICE_H

#include <QObject>
#include <QString>

class UpdateService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool checking READ checking NOTIFY checkingChanged)

public:
    explicit UpdateService(QObject *parent = nullptr);
    
    QString status() const { return m_status; }
    bool checking() const { return m_checking; }
    
    Q_INVOKABLE void checkForUpdates();
    Q_INVOKABLE bool hasUpdates();

signals:
    void statusChanged();
    void checkingChanged();

private:
    QString m_status; // checking, up_to_date, updates_available
    bool m_checking;
    
    // TODO: Integrate with pacman update check
};

#endif // UPDATESERVICE_H

