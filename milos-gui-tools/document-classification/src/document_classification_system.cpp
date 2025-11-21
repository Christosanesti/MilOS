#include "document_classification_system.h"
#include <QDebug>
#include <QStandardPaths>
#include <QDir>

DocumentClassificationSystem::DocumentClassificationSystem(QObject* parent)
    : QObject(parent)
    , m_classifier(new DocumentClassifier(this))
    , m_handler(new ClassificationHandler(this))
    , m_storage(new ClassificationStorage(this))
    , m_dbusInterface(new DocumentClassificationDBusInterface(this))
    , m_auditLogger(new AuditLogger(this))
    , m_initialized(false)
{
}

DocumentClassificationSystem::~DocumentClassificationSystem() {
}

bool DocumentClassificationSystem::initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Initialize audit logger
    if (!m_auditLogger->initialize()) {
        qWarning() << "Failed to initialize audit logger";
        return false;
    }
    
    // Initialize classifier
    if (!m_classifier->initialize()) {
        qWarning() << "Failed to initialize document classifier";
        return false;
    }
    
    // Initialize handler
    if (!m_handler->initialize(m_classifier)) {
        qWarning() << "Failed to initialize classification handler";
        return false;
    }
    
    // Initialize storage
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dbPath);
    dbPath += "/classifications.db";
    
    if (!m_storage->initialize(dbPath)) {
        qWarning() << "Failed to initialize classification storage";
        return false;
    }
    
    // Connect signals for audit logging
    connect(m_classifier, &DocumentClassifier::documentClassified, this, [this](const QString& filePath, ClassificationLevel level) {
        QString levelStr;
        switch (level) {
            case ClassificationLevel::TopSecret:
                levelStr = "TopSecret";
                break;
            case ClassificationLevel::Secret:
                levelStr = "Secret";
                break;
            case ClassificationLevel::Confidential:
                levelStr = "Confidential";
                break;
            default:
                levelStr = "Unclassified";
                break;
        }
        m_auditLogger->logClassification(filePath, levelStr);
        
        // Store classification
        ClassificationMetadata metadata = m_classifier->getClassificationMetadata(filePath);
        m_storage->storeClassification(filePath, metadata);
    });
    
    connect(m_handler, &ClassificationHandler::operationBlocked, this, [this](const QString& filePath, const QString& operation, const QString& reason) {
        m_auditLogger->logOperationBlocked(filePath, operation, reason);
    });
    
    // Initialize D-Bus interface
    m_dbusInterface->setClassifier(m_classifier);
    m_dbusInterface->setHandler(m_handler);
    
    if (!m_dbusInterface->initialize()) {
        qWarning() << "Failed to initialize D-Bus interface";
        return false;
    }
    
    m_initialized = true;
    return true;
}

