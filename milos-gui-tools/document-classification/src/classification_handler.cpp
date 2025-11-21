#include "classification_handler.h"
#include <QDebug>

ClassificationHandler::ClassificationHandler(QObject* parent)
    : QObject(parent)
    , m_classifier(nullptr)
{
}

ClassificationHandler::~ClassificationHandler() {
}

bool ClassificationHandler::initialize(DocumentClassifier* classifier) {
    if (!classifier) {
        return false;
    }
    
    m_classifier = classifier;
    return true;
}

bool ClassificationHandler::isOperationAllowed(const QString& filePath, const QString& operation) const {
    if (!m_classifier) {
        return true;  // Allow if no classifier
    }
    
    ClassificationLevel level = m_classifier->getClassificationLevel(filePath);
    
    if (!isAllowedForLevel(level, operation)) {
        QString reason = QString("Operation '%1' not allowed for %2 classification level")
                        .arg(operation, levelToString(level));
        emit operationBlocked(filePath, operation, reason);
        return false;
    }
    
    return true;
}

QStringList ClassificationHandler::getHandlingInstructions(const QString& filePath) const {
    if (!m_classifier) {
        return QStringList();
    }
    
    ClassificationMetadata metadata = m_classifier->getClassificationMetadata(filePath);
    return metadata.handlingInstructions;
}

QStringList ClassificationHandler::getRequiredPermissions(const QString& filePath) const {
    if (!m_classifier) {
        return QStringList();
    }
    
    ClassificationLevel level = m_classifier->getClassificationLevel(filePath);
    QStringList permissions;
    
    switch (level) {
        case ClassificationLevel::TopSecret:
            permissions << "TopSecretClearance" << "SecureFacilityAccess";
            break;
        case ClassificationLevel::Secret:
            permissions << "SecretClearance" << "ControlledEnvironmentAccess";
            break;
        case ClassificationLevel::Confidential:
            permissions << "ConfidentialClearance";
            break;
        default:
            break;
    }
    
    return permissions;
}

bool ClassificationHandler::isAllowedForLevel(ClassificationLevel level, const QString& operation) const {
    // Define operation restrictions by classification level
    switch (level) {
        case ClassificationLevel::TopSecret:
            // Top Secret: very restrictive
            if (operation == "copy" || operation == "share" || operation == "email" || 
                operation == "print" || operation == "export") {
                return false;
            }
            break;
        case ClassificationLevel::Secret:
            // Secret: moderate restrictions
            if (operation == "share" || operation == "email" || operation == "print") {
                return false;
            }
            break;
        case ClassificationLevel::Confidential:
            // Confidential: light restrictions
            if (operation == "share" || operation == "email") {
                return false;
            }
            break;
        default:
            // Unclassified: no restrictions
            return true;
    }
    
    return true;
}

QStringList ClassificationHandler::getHandlingRules(ClassificationLevel level) const {
    QStringList rules;
    
    switch (level) {
        case ClassificationLevel::TopSecret:
            rules << "Handle in secure facility only"
                  << "Requires Top Secret clearance"
                  << "No electronic transmission"
                  << "Physical access control required";
            break;
        case ClassificationLevel::Secret:
            rules << "Handle in controlled environment"
                  << "Requires Secret clearance"
                  << "Encrypted transmission only"
                  << "Access logging required";
            break;
        case ClassificationLevel::Confidential:
            rules << "Handle with care"
                  << "Requires Confidential clearance"
                  << "Encrypted storage required";
            break;
        default:
            rules << "Standard handling";
            break;
    }
    
    return rules;
}

QString ClassificationHandler::levelToString(ClassificationLevel level) const {
    switch (level) {
        case ClassificationLevel::TopSecret:
            return "Top Secret";
        case ClassificationLevel::Secret:
            return "Secret";
        case ClassificationLevel::Confidential:
            return "Confidential";
        default:
            return "Unclassified";
    }
}

