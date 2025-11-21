#ifndef MILITARY_RADIO_AUDIT_LOGGER_H
#define MILITARY_RADIO_AUDIT_LOGGER_H

#include <QObject>
#include <QString>

class AuditLogger : public QObject {
    Q_OBJECT

public:
    explicit AuditLogger(QObject* parent = nullptr);
    ~AuditLogger();

    bool initialize();
    void logMessageSent(const QString& messageId, const QString& protocol);
    void logMessageReceived(const QString& messageId, const QString& protocol);

private:
    QString generateEventId() const;
};

#endif // MILITARY_RADIO_AUDIT_LOGGER_H

