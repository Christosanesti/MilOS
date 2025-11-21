#ifndef DOCUMENT_CLASSIFICATION_SYSTEM_H
#define DOCUMENT_CLASSIFICATION_SYSTEM_H

#include "document_classifier.h"
#include "classification_handler.h"
#include "classification_storage.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include <QObject>

/**
 * @brief Document Classification System
 * 
 * Main system for document classification and handling.
 */
class DocumentClassificationSystem : public QObject {
    Q_OBJECT

public:
    explicit DocumentClassificationSystem(QObject* parent = nullptr);
    ~DocumentClassificationSystem();

    /**
     * @brief Initialize document classification system
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get document classifier
     * @return Document classifier
     */
    DocumentClassifier* getClassifier() const { return m_classifier; }

    /**
     * @brief Get classification handler
     * @return Classification handler
     */
    ClassificationHandler* getHandler() const { return m_handler; }

    /**
     * @brief Get D-Bus interface
     * @return D-Bus interface
     */
    DocumentClassificationDBusInterface* getDBusInterface() const { return m_dbusInterface; }

private:
    DocumentClassifier* m_classifier;
    ClassificationHandler* m_handler;
    ClassificationStorage* m_storage;
    DocumentClassificationDBusInterface* m_dbusInterface;
    AuditLogger* m_auditLogger;
    bool m_initialized;
};

#endif // DOCUMENT_CLASSIFICATION_SYSTEM_H

