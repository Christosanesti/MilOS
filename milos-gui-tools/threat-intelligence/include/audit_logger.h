#ifndef THREAT_INTELLIGENCE_AUDIT_LOGGER_H
#define THREAT_INTELLIGENCE_AUDIT_LOGGER_H

#include <QObject>
#include <QString>

class AuditLogger : public QObject {
    Q_OBJECT

public:
    explicit AuditLogger(QObject* parent = nullptr);
    ~AuditLogger();

    bool initialize();
    void logFeedUpdate(const QString& feedId, int indicatorCount);
    void logIndicatorBlocked(const QString& indicatorId, const QString& reason);
    void logCorrelationDetected(const QString& correlationId);

private:
    QString generateEventId() const;
};

#endif // THREAT_INTELLIGENCE_AUDIT_LOGGER_H

