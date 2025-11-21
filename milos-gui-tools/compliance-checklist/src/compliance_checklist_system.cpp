#include "compliance_checklist_system.h"
#include <QDebug>

ComplianceChecklistSystem::ComplianceChecklistSystem(QObject* parent)
    : QObject(parent)
    , m_checklistManager(new ComplianceChecklistManager(this))
    , m_reporter(new ComplianceReporter(this))
    , m_dbusInterface(new ComplianceChecklistDBusInterface(this))
    , m_auditLogger(new AuditLogger(this))
    , m_initialized(false)
{
}

ComplianceChecklistSystem::~ComplianceChecklistSystem() {
}

bool ComplianceChecklistSystem::initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Initialize audit logger
    if (!m_auditLogger->initialize()) {
        qWarning() << "Failed to initialize audit logger";
        return false;
    }
    
    // Initialize checklist manager
    if (!m_checklistManager->initialize()) {
        qWarning() << "Failed to initialize checklist manager";
        return false;
    }
    
    // Initialize reporter
    if (!m_reporter->initialize()) {
        qWarning() << "Failed to initialize compliance reporter";
        return false;
    }
    
    // Connect signals for audit logging
    connect(m_checklistManager, &ComplianceChecklistManager::checklistCreated, this, [this](const QString& checklistId) {
        // Get standard from checklist
        ComplianceChecklist checklist = m_checklistManager->getChecklist(checklistId);
        QString standard = "Custom";
        switch (checklist.standard) {
            case ComplianceStandard::NIST:
                standard = "NIST";
                break;
            case ComplianceStandard::CommonCriteria:
                standard = "CommonCriteria";
                break;
            case ComplianceStandard::STIG:
                standard = "STIG";
                break;
            default:
                break;
        }
        m_auditLogger->logChecklistCreated(checklistId, standard);
    });
    
    connect(m_checklistManager, &ComplianceChecklistManager::itemStatusUpdated, this, [this](const QString& checklistId, const QString& itemId, ChecklistItemStatus status) {
        QString statusStr;
        switch (status) {
            case ChecklistItemStatus::NotStarted:
                statusStr = "NotStarted";
                break;
            case ChecklistItemStatus::InProgress:
                statusStr = "InProgress";
                break;
            case ChecklistItemStatus::Completed:
                statusStr = "Completed";
                break;
            case ChecklistItemStatus::Failed:
                statusStr = "Failed";
                break;
            default:
                statusStr = "NotApplicable";
                break;
        }
        m_auditLogger->logItemStatusChanged(checklistId, itemId, statusStr);
    });
    
    // Initialize D-Bus interface
    m_dbusInterface->setChecklistManager(m_checklistManager);
    m_dbusInterface->setReporter(m_reporter);
    
    if (!m_dbusInterface->initialize()) {
        qWarning() << "Failed to initialize D-Bus interface";
        return false;
    }
    
    m_initialized = true;
    return true;
}

