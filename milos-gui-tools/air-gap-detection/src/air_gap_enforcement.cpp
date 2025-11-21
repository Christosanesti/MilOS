#include "air_gap_enforcement.h"
#include <QDebug>

AirGapEnforcement::AirGapEnforcement(QObject* parent)
    : QObject(parent)
    , m_detector(nullptr)
    , m_currentPolicyLevel(SecurityPolicyLevel::Normal)
    , m_dataLeakagePreventionEnabled(false)
{
}

AirGapEnforcement::~AirGapEnforcement() {
}

bool AirGapEnforcement::initialize(AirGapDetector* detector) {
    if (!detector) {
        return false;
    }
    
    m_detector = detector;
    connect(m_detector, &AirGapDetector::statusChanged, this, &AirGapEnforcement::onAirGapStatusChanged);
    
    // Initial policy update
    onAirGapStatusChanged(m_detector->status());
    
    return true;
}

bool AirGapEnforcement::isOperationAllowed(const QString& operation, const QVariantMap& details) const {
    // Check if operation is in blocked list
    if (m_blockedOperations.contains(operation)) {
        return false;
    }
    
    // Check for data leakage operations
    if (m_dataLeakagePreventionEnabled && isDataLeakageOperation(operation)) {
        return false;
    }
    
    return true;
}

QStringList AirGapEnforcement::getBlockedOperations() const {
    return m_blockedOperations;
}

QVariantMap AirGapEnforcement::getPolicyConfiguration() const {
    QVariantMap config;
    config["policy_level"] = static_cast<int>(m_currentPolicyLevel);
    config["data_leakage_prevention"] = m_dataLeakagePreventionEnabled;
    config["blocked_operations"] = m_blockedOperations;
    return config;
}

void AirGapEnforcement::onAirGapStatusChanged(AirGapStatus status) {
    updatePolicyLevel(status);
}

void AirGapEnforcement::updatePolicyLevel(AirGapStatus status) {
    SecurityPolicyLevel oldLevel = m_currentPolicyLevel;
    
    if (status == AirGapStatus::AirGapped) {
        m_currentPolicyLevel = SecurityPolicyLevel::Maximum;
    } else if (status == AirGapStatus::Connected) {
        m_currentPolicyLevel = SecurityPolicyLevel::Normal;
    } else {
        m_currentPolicyLevel = SecurityPolicyLevel::Enhanced;
    }
    
    if (oldLevel != m_currentPolicyLevel) {
        applySecurityPolicies(m_currentPolicyLevel);
        emit policyLevelChanged(m_currentPolicyLevel);
    }
}

void AirGapEnforcement::applySecurityPolicies(SecurityPolicyLevel level) {
    m_blockedOperations.clear();
    
    switch (level) {
        case SecurityPolicyLevel::Normal:
            m_dataLeakagePreventionEnabled = false;
            // No blocked operations in normal mode
            break;
            
        case SecurityPolicyLevel::Enhanced:
            m_dataLeakagePreventionEnabled = true;
            // Block some operations
            m_blockedOperations << "external_file_transfer"
                                << "cloud_sync"
                                << "email_send";
            break;
            
        case SecurityPolicyLevel::Maximum:
            m_dataLeakagePreventionEnabled = true;
            // Block all data leakage operations
            m_blockedOperations << "external_file_transfer"
                                << "cloud_sync"
                                << "email_send"
                                << "usb_mass_storage"
                                << "network_file_sharing"
                                << "remote_desktop"
                                << "vpn_connection"
                                << "wireless_connection";
            break;
    }
    
    emit policyLevelChanged(m_currentPolicyLevel);
}

bool AirGapEnforcement::isDataLeakageOperation(const QString& operation) const {
    QStringList leakageOps = {
        "external_file_transfer",
        "cloud_sync",
        "email_send",
        "usb_mass_storage",
        "network_file_sharing",
        "remote_desktop",
        "vpn_connection",
        "wireless_connection"
    };
    
    return leakageOps.contains(operation);
}

