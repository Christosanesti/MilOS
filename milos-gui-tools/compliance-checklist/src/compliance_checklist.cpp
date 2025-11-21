#include "compliance_checklist.h"
#include <QUuid>
#include <QDebug>
#include <QDateTime>

ComplianceChecklistManager::ComplianceChecklistManager(QObject* parent)
    : QObject(parent)
{
}

ComplianceChecklistManager::~ComplianceChecklistManager() {
}

bool ComplianceChecklistManager::initialize() {
    return true;
}

QStringList ComplianceChecklistManager::availableStandards() const {
    return QStringList() << "NIST" << "CommonCriteria" << "STIG" << "Custom";
}

QString ComplianceChecklistManager::createChecklist(ComplianceStandard standard, const QString& name) {
    ComplianceChecklist checklist;
    checklist.id = generateChecklistId();
    checklist.standard = standard;
    checklist.name = name.isEmpty() ? QString("Checklist %1").arg(checklist.id) : name;
    checklist.description = QString("Compliance checklist for %1 standard").arg(standardToString(standard));
    checklist.items = loadStandardItems(standard);
    checklist.createdAt = QDateTime::currentDateTime();
    checklist.lastUpdated = QDateTime::currentDateTime();
    checklist.completionPercentage = 0;
    
    m_checklists[checklist.id] = checklist;
    
    emit checklistCreated(checklist.id);
    
    return checklist.id;
}

ComplianceChecklist ComplianceChecklistManager::getChecklist(const QString& checklistId) const {
    return m_checklists.value(checklistId, ComplianceChecklist());
}

QList<ComplianceChecklist> ComplianceChecklistManager::getAllChecklists() const {
    return m_checklists.values();
}

bool ComplianceChecklistManager::updateItemStatus(const QString& checklistId, const QString& itemId,
                                                  ChecklistItemStatus status, const QString& notes) {
    if (!m_checklists.contains(checklistId)) {
        return false;
    }
    
    ComplianceChecklist& checklist = m_checklists[checklistId];
    
    for (ChecklistItem& item : checklist.items) {
        if (item.id == itemId) {
            item.status = status;
            item.notes = notes;
            if (status == ChecklistItemStatus::Completed) {
                item.completedAt = QDateTime::currentDateTime();
                item.completedBy = "user";  // In production, would use current user
            }
            
            checklist.lastUpdated = QDateTime::currentDateTime();
            updateProgress(checklistId);
            
            emit itemStatusUpdated(checklistId, itemId, status);
            return true;
        }
    }
    
    return false;
}

bool ComplianceChecklistManager::addEvidence(const QString& checklistId, const QString& itemId, const QString& evidence) {
    if (!m_checklists.contains(checklistId)) {
        return false;
    }
    
    ComplianceChecklist& checklist = m_checklists[checklistId];
    
    for (ChecklistItem& item : checklist.items) {
        if (item.id == itemId) {
            item.evidence.append(evidence);
            checklist.lastUpdated = QDateTime::currentDateTime();
            return true;
        }
    }
    
    return false;
}

int ComplianceChecklistManager::getProgress(const QString& checklistId) const {
    if (!m_checklists.contains(checklistId)) {
        return 0;
    }
    
    return m_checklists[checklistId].completionPercentage;
}

QVariantMap ComplianceChecklistManager::generateReport(const QString& checklistId) const {
    QVariantMap report;
    
    if (!m_checklists.contains(checklistId)) {
        return report;
    }
    
    const ComplianceChecklist& checklist = m_checklists[checklistId];
    
    report["checklist_id"] = checklist.id;
    report["standard"] = static_cast<int>(checklist.standard);
    report["name"] = checklist.name;
    report["description"] = checklist.description;
    report["completion_percentage"] = checklist.completionPercentage;
    report["created_at"] = checklist.createdAt.toString(Qt::ISODate);
    report["last_updated"] = checklist.lastUpdated.toString(Qt::ISODate);
    
    QVariantList items;
    for (const ChecklistItem& item : checklist.items) {
        QVariantMap itemMap;
        itemMap["id"] = item.id;
        itemMap["title"] = item.title;
        itemMap["description"] = item.description;
        itemMap["status"] = static_cast<int>(item.status);
        itemMap["evidence"] = item.evidence;
        itemMap["notes"] = item.notes;
        itemMap["completed_at"] = item.completedAt.toString(Qt::ISODate);
        itemMap["completed_by"] = item.completedBy;
        items.append(itemMap);
    }
    report["items"] = items;
    
    return report;
}

QString ComplianceChecklistManager::generateChecklistId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QList<ChecklistItem> ComplianceChecklistManager::loadStandardItems(ComplianceStandard standard) const {
    QList<ChecklistItem> items;
    
    // Load standard-specific checklist items
    // In production, would load from configuration files or database
    switch (standard) {
        case ComplianceStandard::NIST:
            items.append(createItem("nist-1", "Access Control", "Implement access control measures"));
            items.append(createItem("nist-2", "Encryption", "Encrypt sensitive data at rest and in transit"));
            items.append(createItem("nist-3", "Audit Logging", "Maintain comprehensive audit logs"));
            items.append(createItem("nist-4", "Incident Response", "Establish incident response procedures"));
            items.append(createItem("nist-5", "Security Training", "Provide security awareness training"));
            break;
        case ComplianceStandard::CommonCriteria:
            items.append(createItem("cc-1", "Security Target", "Define security target"));
            items.append(createItem("cc-2", "Security Functions", "Implement security functions"));
            items.append(createItem("cc-3", "Assurance Requirements", "Meet assurance requirements"));
            items.append(createItem("cc-4", "Evaluation", "Complete evaluation process"));
            break;
        case ComplianceStandard::STIG:
            items.append(createItem("stig-1", "System Configuration", "Configure system according to STIG"));
            items.append(createItem("stig-2", "Network Security", "Implement network security controls"));
            items.append(createItem("stig-3", "Access Control", "Enforce access control policies"));
            items.append(createItem("stig-4", "Audit and Accountability", "Enable audit and accountability"));
            break;
        default:
            items.append(createItem("custom-1", "Custom Item 1", "Custom checklist item"));
            break;
    }
    
    return items;
}

ChecklistItem ComplianceChecklistManager::createItem(const QString& id, const QString& title, const QString& description) const {
    ChecklistItem item;
    item.id = id;
    item.title = title;
    item.description = description;
    item.status = ChecklistItemStatus::NotStarted;
    item.completedAt = QDateTime();
    return item;
}

void ComplianceChecklistManager::updateProgress(const QString& checklistId) {
    if (!m_checklists.contains(checklistId)) {
        return;
    }
    
    ComplianceChecklist& checklist = m_checklists[checklistId];
    
    int completed = 0;
    int total = checklist.items.size();
    
    for (const ChecklistItem& item : checklist.items) {
        if (item.status == ChecklistItemStatus::Completed) {
            completed++;
        }
    }
    
    int percentage = total > 0 ? (completed * 100) / total : 0;
    checklist.completionPercentage = percentage;
    
    emit progressChanged(checklistId, percentage);
}

QString ComplianceChecklistManager::standardToString(ComplianceStandard standard) const {
    switch (standard) {
        case ComplianceStandard::NIST:
            return "NIST";
        case ComplianceStandard::CommonCriteria:
            return "Common Criteria";
        case ComplianceStandard::STIG:
            return "STIG";
        default:
            return "Custom";
    }
}

