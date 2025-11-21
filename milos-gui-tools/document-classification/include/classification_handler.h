#ifndef CLASSIFICATION_HANDLER_H
#define CLASSIFICATION_HANDLER_H

#include "document_classifier.h"
#include <QObject>
#include <QString>
#include <QStringList>

/**
 * @brief Classification Handler
 * 
 * Enforces handling rules based on classification levels.
 */
class ClassificationHandler : public QObject {
    Q_OBJECT

public:
    explicit ClassificationHandler(QObject* parent = nullptr);
    ~ClassificationHandler();

    /**
     * @brief Initialize classification handler
     * @param classifier Document classifier
     * @return true if initialization successful, false otherwise
     */
    bool initialize(DocumentClassifier* classifier);

    /**
     * @brief Check if file operation is allowed
     * @param filePath File path
     * @param operation Operation name (copy, move, delete, share, etc.)
     * @return true if allowed, false otherwise
     */
    bool isOperationAllowed(const QString& filePath, const QString& operation) const;

    /**
     * @brief Get handling instructions for file
     * @param filePath File path
     * @return List of handling instructions
     */
    QStringList getHandlingInstructions(const QString& filePath) const;

    /**
     * @brief Get required permissions for file
     * @param filePath File path
     * @return List of required permissions
     */
    QStringList getRequiredPermissions(const QString& filePath) const;

Q_SIGNALS:
    /**
     * @brief Emitted when operation is blocked
     */
    void operationBlocked(const QString& filePath, const QString& operation, const QString& reason);

private:
    DocumentClassifier* m_classifier;
    
    bool isAllowedForLevel(ClassificationLevel level, const QString& operation) const;
    QStringList getHandlingRules(ClassificationLevel level) const;
};

#endif // CLASSIFICATION_HANDLER_H

