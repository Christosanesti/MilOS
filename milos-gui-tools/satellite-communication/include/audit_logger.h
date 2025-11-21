#ifndef SATELLITE_COMMUNICATION_AUDIT_LOGGER_H
#define SATELLITE_COMMUNICATION_AUDIT_LOGGER_H

#include <QObject>
#include <QString>

class AuditLogger : public QObject {
    Q_OBJECT

public:
    explicit AuditLogger(QObject* parent = nullptr);
    ~AuditLogger();

    bool initialize();
    void logMessageSent(const QString& messageId, const QString& protocol);
    void logBandwidthOptimized(double originalSize, double optimizedSize);

private:
    QString generateEventId() const;
};

#endif // SATELLITE_COMMUNICATION_AUDIT_LOGGER_H

