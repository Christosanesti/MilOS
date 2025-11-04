#ifndef BATCHPROGRESSWIDGET_H
#define BATCHPROGRESSWIDGET_H

#include <QWidget>
#include <QString>
#include <QStringList>
#include <QList>
#include "dbus/encryptionmanager_interface.h"

QT_BEGIN_NAMESPACE
class QProgressBar;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QPropertyAnimation;
QT_END_NAMESPACE

/**
 * @brief File status enumeration for batch progress tracking
 */
enum class FileStatus {
    PENDING,
    IN_PROGRESS,
    SUCCESS,
    FAILED
};

/**
 * @brief File status information structure
 */
struct FileStatusInfo {
    QString filePath;
    QString fileName;
    int fileIndex;  // 1-based index for display
    FileStatus status;
    int progressPercentage;  // 0-100 for current file
    QString errorMessage;
    
    FileStatusInfo() : fileIndex(0), status(FileStatus::PENDING), progressPercentage(0) {}
    FileStatusInfo(const QString &path, int index)
        : filePath(path), fileIndex(index), status(FileStatus::PENDING), progressPercentage(0) {
        // Extract file name from path
        int lastSlash = path.lastIndexOf('/');
        fileName = (lastSlash >= 0) ? path.mid(lastSlash + 1) : path;
    }
};

/**
 * @brief Batch progress tracking UI widget
 * 
 * This widget displays real-time progress for batch encryption operations.
 * It subscribes to D-Bus signals and updates the UI accordingly.
 */
class BatchProgressWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BatchProgressWidget(QWidget *parent = nullptr);
    ~BatchProgressWidget();

    /**
     * @brief Initialize progress tracking for a new batch operation
     * @param operationId Operation ID for the batch
     * @param filePaths List of file paths in the batch
     */
    void initializeBatch(const QString &operationId, const QStringList &filePaths);

    /**
     * @brief Get current operation ID
     * @return Operation ID string
     */
    QString getOperationId() const { return m_operationId; }

    /**
     * @brief Check if a batch operation is currently active
     * @return True if operation is in progress
     */
    bool isOperationActive() const { return !m_operationId.isEmpty(); }

    /**
     * @brief Connect to D-Bus signals for batch encryption progress
     * @param serviceName D-Bus service name (default: "org.milos.EncryptionManager")
     * @param objectPath D-Bus object path (default: "/org/milos/EncryptionManager")
     * @return True if connection successful
     */
    bool connectToDBusSignals(const QString &serviceName = "org.milos.EncryptionManager",
                              const QString &objectPath = "/org/milos/EncryptionManager");

    /**
     * @brief Disconnect from D-Bus signals
     */
    void disconnectFromDBusSignals();

public slots:
    /**
     * @brief Handle batch encryption started signal
     * @param operationId Operation ID
     */
    void onBatchEncryptionStarted(const QString &operationId);

    /**
     * @brief Handle batch encryption progress signal
     * @param operationId Operation ID
     * @param fileIndex File index (0-based from D-Bus, converted to 1-based for display)
     * @param percentage Progress percentage for current file (0-100)
     */
    void onBatchEncryptionProgress(const QString &operationId, int fileIndex, int percentage);

    /**
     * @brief Handle batch encryption completed signal
     * @param operationId Operation ID
     * @param status Operation status ("SUCCESS", "FAILED", "CANCELLED")
     */
    void onBatchEncryptionCompleted(const QString &operationId, const QString &status);

    /**
     * @brief Handle encryption error signal
     * @param operationId Operation ID
     * @param errorMessage Error message
     */
    void onEncryptionError(const QString &operationId, const QString &errorMessage);

signals:
    /**
     * @brief Emitted when batch operation completes
     * @param operationId Operation ID
     * @param status Operation status
     */
    void batchCompleted(const QString &operationId, const QString &status);

private slots:
    /**
     * @brief Handle cancel button click
     */
    void onCancelClicked();

    /**
     * @brief Handle file item click for error details
     * @param item Clicked list item
     */
    void onFileItemClicked(QListWidgetItem *item);

private:
    /**
     * @brief Setup UI components
     */
    void setupUI();

    /**
     * @brief Update overall progress percentage
     */
    void updateOverallProgress();

    /**
     * @brief Update current file indicator text
     */
    void updateCurrentFileIndicator();

    /**
     * @brief Update file status indicator in list
     * @param fileIndex File index (1-based)
     * @param status New status
     * @param progressPercentage Progress percentage (0-100)
     */
    void updateFileStatus(int fileIndex, FileStatus status, int progressPercentage = 0);

    /**
     * @brief Apply xenon visual effects to a file status indicator
     * @param item List widget item
     * @param status Current file status
     */
    void applyXenonEffect(QListWidgetItem *item, FileStatus status);

    /**
     * @brief Create visual indicator for file status
     * @param status File status
     * @return Status text and color/style information
     */
    QString getStatusText(FileStatus status) const;
    QString getStatusStyle(FileStatus status) const;

    /**
     * @brief Reset widget to initial state
     */
    void reset();

    /**
     * @brief Validate operation ID before processing updates
     * @param operationId Operation ID to validate
     * @return True if operation ID matches current operation
     */
    bool validateOperationId(const QString &operationId) const;

    QString m_operationId;
    QStringList m_filePaths;
    QList<FileStatusInfo> m_fileStatuses;
    
    // D-Bus connection
    OrgMilosEncryptionManagerInterface *m_dbusInterface;
    
    // UI Components
    QVBoxLayout *m_mainLayout;
    QLabel *m_titleLabel;
    QLabel *m_currentFileLabel;
    QProgressBar *m_overallProgressBar;
    QListWidget *m_fileListWidget;
    QPushButton *m_cancelButton;
    QHBoxLayout *m_buttonLayout;
    
    // Xenon effect animations (using Qt animations as placeholder)
    QList<QPropertyAnimation*> m_activeAnimations;
};

#endif // BATCHPROGRESSWIDGET_H

