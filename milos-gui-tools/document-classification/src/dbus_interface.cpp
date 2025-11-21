#include "dbus_interface.h"
#include "document_classifier.h"
#include "classification_handler.h"
#include <QDebug>
#include <QDBusConnection>

DocumentClassificationDBusInterface::DocumentClassificationDBusInterface(QObject* parent)
    : QObject(parent)
    , m_classifier(nullptr)
    , m_handler(nullptr)
{
}

DocumentClassificationDBusInterface::~DocumentClassificationDBusInterface() {
}

bool DocumentClassificationDBusInterface::initialize() {
    if (!QDBusConnection::sessionBus().registerService("org.milos.DocumentClassification")) {
        qWarning() << "Failed to register D-Bus service";
        return false;
    }
    
    if (!QDBusConnection::sessionBus().registerObject("/org/milos/DocumentClassification", this,
                                                        QDBusConnection::ExportAllSlots |
                                                        QDBusConnection::ExportAllSignals)) {
        qWarning() << "Failed to register D-Bus object";
        return false;
    }
    
    return true;
}

void DocumentClassificationDBusInterface::setClassifier(DocumentClassifier* classifier) {
    m_classifier = classifier;
}

void DocumentClassificationDBusInterface::setHandler(ClassificationHandler* handler) {
    m_handler = handler;
}

QString DocumentClassificationDBusInterface::ClassifyDocument(const QString& filePath) {
    if (!m_classifier) {
        return "Unclassified";
    }
    
    ClassificationMetadata metadata = m_classifier->classifyDocument(filePath);
    
    switch (metadata.level) {
        case ClassificationLevel::TopSecret:
            return "TopSecret";
        case ClassificationLevel::Secret:
            return "Secret";
        case ClassificationLevel::Confidential:
            return "Confidential";
        default:
            return "Unclassified";
    }
}

QString DocumentClassificationDBusInterface::GetClassificationLevel(const QString& filePath) {
    if (!m_classifier) {
        return "Unclassified";
    }
    
    ClassificationLevel level = m_classifier->getClassificationLevel(filePath);
    
    switch (level) {
        case ClassificationLevel::TopSecret:
            return "TopSecret";
        case ClassificationLevel::Secret:
            return "Secret";
        case ClassificationLevel::Confidential:
            return "Confidential";
        default:
            return "Unclassified";
    }
}

QVariantMap DocumentClassificationDBusInterface::GetClassificationMetadata(const QString& filePath) {
    QVariantMap result;
    
    if (!m_classifier) {
        return result;
    }
    
    ClassificationMetadata metadata = m_classifier->getClassificationMetadata(filePath);
    
    result["level"] = static_cast<int>(metadata.level);
    result["category"] = metadata.category;
    result["owner"] = metadata.owner;
    result["classified_at"] = metadata.classifiedAt.toString(Qt::ISODate);
    result["classified_by"] = metadata.classifiedBy;
    result["handling_instructions"] = metadata.handlingInstructions;
    result["custom_metadata"] = metadata.customMetadata;
    
    return result;
}

bool DocumentClassificationDBusInterface::SetClassification(const QString& filePath, int level, const QString& category) {
    if (!m_classifier) {
        return false;
    }
    
    ClassificationMetadata metadata;
    metadata.level = static_cast<ClassificationLevel>(level);
    metadata.category = category;
    metadata.classifiedAt = QDateTime::currentDateTime();
    metadata.classifiedBy = "user";  // In production, would use current user
    
    return m_classifier->setClassification(filePath, metadata);
}

bool DocumentClassificationDBusInterface::IsOperationAllowed(const QString& filePath, const QString& operation) {
    if (!m_handler) {
        return true;
    }
    
    return m_handler->isOperationAllowed(filePath, operation);
}

QStringList DocumentClassificationDBusInterface::GetHandlingInstructions(const QString& filePath) {
    if (!m_handler) {
        return QStringList();
    }
    
    return m_handler->getHandlingInstructions(filePath);
}

