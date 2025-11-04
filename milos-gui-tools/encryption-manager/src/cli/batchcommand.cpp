#include "batchcommand.h"
#include "dbus/encryptionmanager_interface.h"
#include <QtCore/QCoreApplication>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include <QtDBus/QDBusPendingReply>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <QtCore/QFileInfo>
#include <iostream>

BatchCommand::BatchCommand(QObject *parent)
    : QObject(parent)
    , m_dbusInterface(nullptr)
    , m_currentFileIndex(-1)
    , m_operationCompleted(false)
    , m_exitCode(1)
{
    connectToDBusSignals();
}

bool BatchCommand::connectToDBusSignals()
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    
    // Check if service is available
    if (!connection.isConnected()) {
        std::cerr << "Error: Cannot connect to D-Bus session bus" << std::endl;
        return false;
    }

    // Create D-Bus interface
    m_dbusInterface = new OrgMilosEncryptionManagerInterface(
        "org.milos.EncryptionManager",
        "/org/milos/EncryptionManager",
        connection,
        this
    );

    // Connect to batch encryption signals
    connect(m_dbusInterface, &OrgMilosEncryptionManagerInterface::BatchEncryptionStarted,
            this, &BatchCommand::onBatchEncryptionStarted);
    connect(m_dbusInterface, &OrgMilosEncryptionManagerInterface::BatchEncryptionProgress,
            this, &BatchCommand::onBatchEncryptionProgress);
    connect(m_dbusInterface, &OrgMilosEncryptionManagerInterface::BatchEncryptionCompleted,
            this, &BatchCommand::onBatchEncryptionCompleted);
    connect(m_dbusInterface, &OrgMilosEncryptionManagerInterface::EncryptionError,
            this, &BatchCommand::onEncryptionError);

    return true;
}

bool BatchCommand::isServiceAvailable() const
{
    if (!m_dbusInterface) {
        return false;
    }

    QDBusConnection connection = QDBusConnection::sessionBus();
    return connection.isConnected() && 
           connection.interface()->isServiceRegistered("org.milos.EncryptionManager");
}

int BatchCommand::execute(const QStringList &filePaths, const QString &algorithm, const QString &keyId)
{
    // Validate file count
    if (filePaths.isEmpty()) {
        std::cerr << "Error: No files specified" << std::endl;
        return 1;
    }

    if (filePaths.size() > 5) {
        std::cerr << "Error: Maximum 5 files allowed for batch operations" << std::endl;
        return 1;
    }

    // Check service availability
    if (!isServiceAvailable()) {
        std::cerr << "Error: Encryption Manager D-Bus service is not available" << std::endl;
        std::cerr << "Please ensure the Encryption Manager service is running" << std::endl;
        return 1;
    }

    // Initialize state
    m_filePaths = filePaths;
    m_fileResults.clear();
    m_fileErrors.clear();
    m_fileResults.resize(filePaths.size(), false);
    m_fileErrors.resize(filePaths.size(), QString());
    m_currentFileIndex = -1;
    m_operationCompleted = false;
    m_exitCode = 1;

    // Output batch start message
    std::cout << "Batch encryption started (" << filePaths.size() << " file";
    if (filePaths.size() != 1) {
        std::cout << "s";
    }
    std::cout << ")" << std::endl;

    // Call D-Bus method
    QDBusPendingReply<QString> reply = m_dbusInterface->EncryptFiles(filePaths, algorithm, keyId);
    
    // Wait for reply
    reply.waitForFinished();

    if (reply.isError()) {
        QDBusError error = reply.error();
        std::cerr << "Error: " << error.message().toStdString() << std::endl;
        return 1;
    }

    // Get operation ID from reply
    m_operationId = reply.value();

    // Process events and wait for completion
    QEventLoop loop;
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);
    connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
    
    // Connect completion signal (using lambda to capture loop and timer)
    auto completionHandler = [&loop, &timeoutTimer](const QString &, const QString &) {
        timeoutTimer.stop();
        loop.quit();
    };
    connect(this, &BatchCommand::batchOperationCompleted, completionHandler);
    
    // Set timeout (30 seconds per file, max 5 files = 300 seconds)
    timeoutTimer.start(300000);

    // Wait for completion
    loop.exec();

    // Calculate summary
    int successCount = 0;
    int failureCount = 0;
    for (bool result : m_fileResults) {
        if (result) {
            successCount++;
        } else {
            failureCount++;
        }
    }

    // Output summary
    outputSummary(successCount, failureCount);

    // Set exit code (0 = all success, 1 = any failure)
    m_exitCode = (failureCount == 0) ? 0 : 1;

    return m_exitCode;
}

void BatchCommand::onBatchEncryptionStarted(const QString &operationId)
{
    if (operationId != m_operationId) {
        return;
    }
    // Batch started - progress will come via BatchEncryptionProgress
}

void BatchCommand::onBatchEncryptionProgress(const QString &operationId, int fileIndex, int percentage)
{
    if (operationId != m_operationId) {
        return;
    }

    // Update current file index
    if (fileIndex >= 0 && fileIndex < m_filePaths.size()) {
        m_currentFileIndex = fileIndex;
        
        // Output progress for current file
        if (percentage == 0) {
            // File encryption started
            outputProgress(fileIndex + 1, m_filePaths.size(), m_filePaths[fileIndex], "Encrypting...");
        } else if (percentage == 100) {
            // File encryption completed successfully
            m_fileResults[fileIndex] = true;
            outputProgress(fileIndex + 1, m_filePaths.size(), m_filePaths[fileIndex], "Success");
        }
    }
}

void BatchCommand::onBatchEncryptionCompleted(const QString &operationId, const QString &status)
{
    if (operationId != m_operationId) {
        return;
    }

    m_operationCompleted = true;

    // Mark any remaining files as completed based on status
    for (int i = 0; i < m_fileResults.size(); ++i) {
        if (m_fileResults[i] == false && m_fileErrors[i].isEmpty()) {
            // File was not explicitly marked as failed, check status
            if (status == "SUCCESS" || status == "PARTIAL_SUCCESS") {
                // If this file was processed, it succeeded
                // Otherwise, mark as failed if batch failed completely
                if (status == "FAILED") {
                    m_fileResults[i] = false;
                    m_fileErrors[i] = "Batch operation failed";
                }
            } else if (status == "FAILED") {
                m_fileResults[i] = false;
                m_fileErrors[i] = "Batch operation failed";
            }
        }
    }

    emit batchOperationCompleted(operationId, status);
}

void BatchCommand::onEncryptionError(const QString &operationId, const QString &errorMessage)
{
    if (operationId != m_operationId) {
        return;
    }

    // Mark current file as failed
    if (m_currentFileIndex >= 0 && m_currentFileIndex < m_filePaths.size()) {
        m_fileResults[m_currentFileIndex] = false;
        m_fileErrors[m_currentFileIndex] = errorMessage;
        
        // Output error status
        outputProgress(m_currentFileIndex + 1, m_filePaths.size(), 
                      m_filePaths[m_currentFileIndex], 
                      QString("Failed: %1").arg(errorMessage));
    }
}

void BatchCommand::outputProgress(int fileIndex, int totalFiles, const QString &filePath, const QString &status)
{
    std::cout << "[" << fileIndex << "/" << totalFiles << "] ";
    std::cout << "Encrypting: " << filePath.toStdString() << " ... " << status.toStdString() << std::endl;
}

void BatchCommand::outputSummary(int successCount, int failureCount)
{
    std::cout << std::endl;
    std::cout << "Batch encryption completed: " << successCount << " succeeded";
    if (failureCount > 0) {
        std::cout << ", " << failureCount << " failed";
    }
    std::cout << std::endl;

    // Output detailed error messages if any
    if (failureCount > 0) {
        std::cout << std::endl << "Failed files:" << std::endl;
        for (int i = 0; i < m_filePaths.size(); ++i) {
            if (!m_fileResults[i]) {
                std::cout << "  - " << m_filePaths[i].toStdString();
                if (!m_fileErrors[i].isEmpty()) {
                    std::cout << ": " << m_fileErrors[i].toStdString();
                }
                std::cout << std::endl;
            }
        }
    }
}

