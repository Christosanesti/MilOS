#ifndef DRONE_UAV_AUDIT_LOGGER_H
#define DRONE_UAV_AUDIT_LOGGER_H

#include <QObject>
#include <QString>

class AuditLogger : public QObject {
    Q_OBJECT

public:
    explicit AuditLogger(QObject* parent = nullptr);
    ~AuditLogger();

    bool initialize();
    void logDroneCommand(const QString& droneId, const QString& command);
    void logMissionExecuted(const QString& missionId, const QString& droneId);

private:
    QString generateEventId() const;
};

#endif // DRONE_UAV_AUDIT_LOGGER_H

