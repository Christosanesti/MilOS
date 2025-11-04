#include "batchencryptionprocessor.h"
#include "singlefileencryption.h"
#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusMessage>
#include <QtCore/QVariantMap>
#include <QtCore/QUuid>

BatchEncryptionProcessor::BatchEncryptionProcessor(QObject *parent)
    : QObject(parent)
{
}

BatchEncryptionResult BatchEncryptionProcessor::processBatchEncryption(const QStringList &filePaths,
                                                                        const QString &algorithm,
                                                                        const QString &keyId)
{
    BatchEncryptionResult result;
    
    // Generate batch operation ID
    result.batchOperationId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    result.totalFiles = filePaths.size();
    
    // Process files sequentially (one at a time)
    for (int i = 0; i < filePaths.size(); ++i) {
        const QString &filePath = filePaths[i];
        
        // Emit signal for file encryption started
        emit fileEncryptionStarted(result.batchOperationId, i, filePath);
        
        // Emit progress signal (0% - starting)
        emit fileEncryptionProgress(result.batchOperationId, i, 0);
        
        // Encrypt file using existing encryption logic
        EncryptionResult encryptionResult = encryptFile(filePath, algorithm, keyId);
        
        // Emit progress signal (50% - encryption in progress)
        emit fileEncryptionProgress(result.batchOperationId, i, 50);
        
        // Create file result
        FileResult fileResult;
        fileResult.filePath = filePath;
        
        if (encryptionResult.success) {
            // File encrypted successfully
            fileResult.status = "SUCCESS";
            result.successfulFiles++;
            
            // Emit progress signal (100% - completed)
            emit fileEncryptionProgress(result.batchOperationId, i, 100);
            emit fileEncryptionCompleted(result.batchOperationId, i, true, QString());
            
            // Log successful encryption to audit service
            logAuditEvent(filePath, algorithm, keyId, "SUCCESS");
        } else {
            // File encryption failed
            fileResult.status = "FAILED";
            fileResult.errorMessage = encryptionResult.errorMessage;
            result.failedFiles++;
            
            // Emit error signal
            emit fileEncryptionCompleted(result.batchOperationId, i, false, encryptionResult.errorMessage);
            
            // Log failed encryption to audit service
            logAuditEvent(filePath, algorithm, keyId, "FAILED", encryptionResult.errorMessage);
        }
        
        result.fileResults.append(fileResult);
        result.processedFiles++;
        
        // Continue processing remaining files even if this one failed (partial success support)
    }
    
    return result;
}

void BatchEncryptionProcessor::logAuditEvent(const QString &filePath,
                                              const QString &algorithm,
                                              const QString &keyId,
                                              const QString &status,
                                              const QString &errorMessage)
{
    // TODO: Integrate with actual audit service D-Bus interface when available
    // For MVP, this is a placeholder that will be connected to audit service in future stories
    
    // Audit service D-Bus interface: org.milos.AuditService
    // Method: LogEvent(event_type: String, event_data: Map<String, Any>)
    // Event Type: "encryption_operation"
    // Event Data: {file_path, algorithm, key_id, status, timestamp, error_message}
    
    QDBusConnection connection = QDBusConnection::sessionBus();
    
    if (!connection.isConnected()) {
        // D-Bus not available, skip audit logging
        // In production, this should queue audit events for retry
        return;
    }
    
    // Prepare event data
    QVariantMap eventData;
    eventData["file_path"] = filePath;
    eventData["algorithm"] = algorithm;
    eventData["key_id"] = keyId;
    eventData["status"] = status;
    eventData["timestamp"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    
    if (!errorMessage.isEmpty()) {
        eventData["error_message"] = errorMessage;
    }
    
    // Create D-Bus interface for audit service
    QDBusInterface auditInterface("org.milos.AuditService",
                                   "/org/milos/AuditService",
                                   "org.milos.AuditService",
                                   connection,
                                   this);
    
    if (auditInterface.isValid()) {
        // Call LogEvent method
        QDBusMessage reply = auditInterface.call("LogEvent", 
                                                  QString("encryption_operation"),
                                                  QVariant::fromValue(eventData));
        
        if (reply.type() == QDBusMessage::ErrorMessage) {
            // Audit logging failed, but don't fail the encryption operation
            // Log error for debugging (in production, use proper logging)
            qWarning("Failed to log audit event: %s", qPrintable(reply.errorMessage()));
        }
    } else {
        // Audit service not available (acceptable for MVP)
        // In production, this should queue audit events for retry
        qWarning("Audit service not available - audit logging skipped");
    }
}

