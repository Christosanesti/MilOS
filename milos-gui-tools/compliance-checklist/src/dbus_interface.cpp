#include "dbus_interface.h"
#include "compliance_checklist.h"
#include "compliance_reporter.h"
#include <QDebug>
#include <QDBusConnection>

ComplianceChecklistDBusInterface::ComplianceChecklistDBusInterface(QObject* parent)
    : QObject(parent)
    , m_checklistManager(nullptr)
    , m_reporter(nullptr)
{
}

ComplianceChecklistDBusInterface::~ComplianceChecklistDBusInterface() {
}

bool ComplianceChecklistDBusInterface::initialize() {
    if (!QDBusConnection::sessionBus().registerService("org.milos.ComplianceChecklist")) {
        qWarning() << "Failed to register D-Bus service";
        return false;
    }
    
    if (!QDBusConnection::sessionBus().registerObject("/org/milos/ComplianceChecklist", this,
                                                        QDBusConnection::ExportAllSlots |
                                                        QDBusConnection::ExportAllSignals)) {
        qWarning() << "Failed to register D-Bus object";
        return false;
    }
    
    return true;
}

void ComplianceChecklistDBusInterface::setChecklistManager(ComplianceChecklistManager* manager) {
    m_checklistManager = manager;
}

void ComplianceChecklistDBusInterface::setReporter(ComplianceReporter* reporter) {
    m_reporter = reporter;
}

QStringList ComplianceChecklistDBusInterface::GetAvailableStandards() {
    if (!m_checklistManager) {
        return QStringList();
    }
    
    return m_checklistManager->availableStandards();
}

QString ComplianceChecklistDBusInterface::CreateChecklist(const QString& standard, const QString& name) {
    if (!m_checklistManager) {
        return QString();
    }
    
    ComplianceStandard std = ComplianceStandard::Custom;
    if (standard == "NIST") {
        std = ComplianceStandard::NIST;
    } else if (standard == "CommonCriteria") {
        std = ComplianceStandard::CommonCriteria;
    } else if (standard == "STIG") {
        std = ComplianceStandard::STIG;
    }
    
    return m_checklistManager->createChecklist(std, name);
}

QVariantMap ComplianceChecklistDBusInterface::GetChecklist(const QString& checklistId) {
    if (!m_checklistManager) {
        return QVariantMap();
    }
    
    return m_checklistManager->generateReport(checklistId);
}

QStringList ComplianceChecklistDBusInterface::GetAllChecklists() {
    if (!m_checklistManager) {
        return QStringList();
    }
    
    QStringList ids;
    QList<ComplianceChecklist> checklists = m_checklistManager->getAllChecklists();
    for (const ComplianceChecklist& checklist : checklists) {
        ids.append(checklist.id);
    }
    
    return ids;
}

bool ComplianceChecklistDBusInterface::UpdateItemStatus(const QString& checklistId, const QString& itemId, int status, const QString& notes) {
    if (!m_checklistManager) {
        return false;
    }
    
    return m_checklistManager->updateItemStatus(checklistId, itemId, static_cast<ChecklistItemStatus>(status), notes);
}

bool ComplianceChecklistDBusInterface::AddEvidence(const QString& checklistId, const QString& itemId, const QString& evidence) {
    if (!m_checklistManager) {
        return false;
    }
    
    return m_checklistManager->addEvidence(checklistId, itemId, evidence);
}

int ComplianceChecklistDBusInterface::GetProgress(const QString& checklistId) {
    if (!m_checklistManager) {
        return 0;
    }
    
    return m_checklistManager->getProgress(checklistId);
}

QString ComplianceChecklistDBusInterface::GenerateReport(const QString& checklistId, const QString& format) {
    if (!m_reporter) {
        return QString();
    }
    
    return m_reporter->generateReport(checklistId, format);
}

QVariantMap ComplianceChecklistDBusInterface::GenerateGapAnalysis(const QString& checklistId) {
    if (!m_reporter) {
        return QVariantMap();
    }
    
    return m_reporter->generateGapAnalysis(checklistId);
}

