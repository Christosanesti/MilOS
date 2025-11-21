#ifndef SIGNAL_JAMMING_AUDIT_LOGGER_H
#define SIGNAL_JAMMING_AUDIT_LOGGER_H

#include <QObject>
#include <QString>

class AuditLogger : public QObject {
    Q_OBJECT

public:
    explicit AuditLogger(QObject* parent = nullptr);
    ~AuditLogger();

    bool initialize();
    void logJammingStarted(const QString& deviceId, const QString& patternId);
    void logJammingStopped(const QString& deviceId);
    void logDeviceConnected(const QString& deviceId);
    void logDeviceDisconnected(const QString& deviceId);

private:
    QString generateEventId() const;
};

#endif // SIGNAL_JAMMING_AUDIT_LOGGER_H

