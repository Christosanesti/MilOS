#ifndef COMPLIANCE_CHECKLIST_DBUS_INTERFACE_H
#define COMPLIANCE_CHECKLIST_DBUS_INTERFACE_H

#include "compliance_checklist.h"
#include "compliance_reporter.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

class ComplianceChecklistManager;
class ComplianceReporter;

/**
 * @brief Compliance Checklist D-Bus Interface
 */
class ComplianceChecklistDBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.ComplianceChecklist")

public:
    explicit ComplianceChecklistDBusInterface(QObject* parent = nullptr);
    ~ComplianceChecklistDBusInterface();

    bool initialize();
    void setChecklistManager(ComplianceChecklistManager* manager);
    void setReporter(ComplianceReporter* reporter);

public slots:
    QStringList GetAvailableStandards();
    QString CreateChecklist(const QString& standard, const QString& name);
    QVariantMap GetChecklist(const QString& checklistId);
    QStringList GetAllChecklists();
    bool UpdateItemStatus(const QString& checklistId, const QString& itemId, int status, const QString& notes);
    bool AddEvidence(const QString& checklistId, const QString& itemId, const QString& evidence);
    int GetProgress(const QString& checklistId);
    QString GenerateReport(const QString& checklistId, const QString& format);
    QVariantMap GenerateGapAnalysis(const QString& checklistId);

private:
    ComplianceChecklistManager* m_checklistManager;
    ComplianceReporter* m_reporter;
};

#endif // COMPLIANCE_CHECKLIST_DBUS_INTERFACE_H

