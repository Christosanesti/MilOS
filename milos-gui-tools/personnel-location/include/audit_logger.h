#ifndef PERSONNEL_LOCATION_AUDIT_LOGGER_H
#define PERSONNEL_LOCATION_AUDIT_LOGGER_H

#include <QObject>
#include <QString>

class AuditLogger : public QObject {
    Q_OBJECT

public:
    explicit AuditLogger(QObject* parent = nullptr);
    ~AuditLogger();

    bool initialize();
    void logLocationUpdate(const QString& personnelId, const QString& location);
    void logTrackingStarted(const QString& personnelId);
    void logTrackingStopped(const QString& personnelId);

private:
    QString generateEventId() const;
};

#endif // PERSONNEL_LOCATION_AUDIT_LOGGER_H

