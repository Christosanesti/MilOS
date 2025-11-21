#ifndef COMPLIANCE_CHECKLIST_AUDIT_LOGGER_H
#define COMPLIANCE_CHECKLIST_AUDIT_LOGGER_H

#include <QObject>
#include <QString>

class AuditLogger : public QObject {
    Q_OBJECT

public:
    explicit AuditLogger(QObject* parent = nullptr);
    ~AuditLogger();

    bool initialize();
    void logChecklistCreated(const QString& checklistId, const QString& standard);
    void logItemStatusChanged(const QString& checklistId, const QString& itemId, const QString& status);
    void logReportGenerated(const QString& checklistId, const QString& format);

private:
    QString generateEventId() const;
};

#endif // COMPLIANCE_CHECKLIST_AUDIT_LOGGER_H

