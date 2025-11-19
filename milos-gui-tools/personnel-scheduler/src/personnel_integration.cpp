#include "personnel_integration.h"
#include <QDebug>

PersonnelIntegration::PersonnelIntegration(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
{
}

PersonnelIntegration::~PersonnelIntegration() {
}

bool PersonnelIntegration::initialize(const QString& apiEndpoint, const QString& apiKey) {
    if (apiEndpoint.isEmpty() || apiKey.isEmpty()) {
        return false;
    }
    
    m_apiEndpoint = apiEndpoint;
    m_apiKey = apiKey;
    m_initialized = true;
    
    return true;
}

bool PersonnelIntegration::syncPersonnelData() {
    if (!m_initialized) {
        emit syncFailed("Integration not initialized");
        return false;
    }
    
    // This would call external API to sync personnel data
    // For now, placeholder implementation
    QVariantMap personnel1;
    personnel1["personnel_id"] = "P001";
    personnel1["name"] = "John Doe";
    personnel1["department"] = "Security";
    m_personnelData.append(personnel1);
    
    emit personnelDataSynced();
    return true;
}

QVariantMap PersonnelIntegration::getPersonnelInfo(const QString& personnelId) const {
    for (const QVariantMap& personnel : m_personnelData) {
        if (personnel["personnel_id"].toString() == personnelId) {
            return personnel;
        }
    }
    
    return QVariantMap();
}

QList<QVariantMap> PersonnelIntegration::getAllPersonnel() const {
    return m_personnelData;
}

