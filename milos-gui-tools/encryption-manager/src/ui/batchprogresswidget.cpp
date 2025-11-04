#include "batchprogresswidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QGraphicsEffect>
#include <QColor>
#include <QFont>
#include <QBrush>
#include <QMessageBox>
#include <QFileInfo>
#include <QAbstractItemView>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>

BatchProgressWidget::BatchProgressWidget(QWidget *parent)
    : QWidget(parent)
    , m_operationId()
    , m_dbusInterface(nullptr)
    , m_mainLayout(new QVBoxLayout(this))
    , m_titleLabel(new QLabel(tr("Batch Encryption Progress"), this))
    , m_currentFileLabel(new QLabel(tr("Preparing batch operation..."), this))
    , m_overallProgressBar(new QProgressBar(this))
    , m_fileListWidget(new QListWidget(this))
    , m_cancelButton(new QPushButton(tr("Cancel"), this))
    , m_buttonLayout(new QHBoxLayout())
{
    setupUI();
}

BatchProgressWidget::~BatchProgressWidget()
{
    // Disconnect from D-Bus
    disconnectFromDBusSignals();
    
    // Clean up animations
    for (auto *animation : m_activeAnimations) {
        if (animation) {
            animation->stop();
            animation->deleteLater();
        }
    }
}

void BatchProgressWidget::setupUI()
{
    m_mainLayout->setContentsMargins(15, 15, 15, 15);
    m_mainLayout->setSpacing(15);

    // Title
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(m_titleLabel);

    // Current file indicator
    m_currentFileLabel->setAlignment(Qt::AlignCenter);
    QFont currentFileFont = m_currentFileLabel->font();
    currentFileFont.setPointSize(12);
    m_currentFileLabel->setFont(currentFileFont);
    m_mainLayout->addWidget(m_currentFileLabel);

    // Overall progress bar
    m_overallProgressBar->setMinimum(0);
    m_overallProgressBar->setMaximum(100);
    m_overallProgressBar->setValue(0);
    m_overallProgressBar->setTextVisible(true);
    m_overallProgressBar->setFormat(tr("%p% Complete"));
    m_overallProgressBar->setStyleSheet(
        "QProgressBar {"
        "    border: 2px solid #4A90E2;"
        "    border-radius: 5px;"
        "    text-align: center;"
        "    background-color: #F0F8FF;"
        "    height: 25px;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #4A90E2, stop:1 #50C878);"
        "    border-radius: 3px;"
        "}"
    );
    m_mainLayout->addWidget(m_overallProgressBar);

    // File list widget
    m_fileListWidget->setMinimumHeight(200);
    m_fileListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_fileListWidget->setStyleSheet(
        "QListWidget {"
        "    border: 1px solid #CCCCCC;"
        "    border-radius: 5px;"
        "    background-color: #FAFAFA;"
        "}"
        "QListWidget::item {"
        "    border-bottom: 1px solid #EEEEEE;"
        "    padding: 5px;"
        "}"
        "QListWidget::item:selected {"
        "    background-color: #E3F2FD;"
        "}"
    );
    m_mainLayout->addWidget(m_fileListWidget);

    // Button layout
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_cancelButton);
    m_buttonLayout->addStretch();
    m_mainLayout->addLayout(m_buttonLayout);

    // Connect signals
    connect(m_cancelButton, &QPushButton::clicked, this, &BatchProgressWidget::onCancelClicked);
    connect(m_fileListWidget, &QListWidget::itemClicked, this, &BatchProgressWidget::onFileItemClicked);

    // Initial state
    reset();
    
    // Connect to D-Bus signals
    connectToDBusSignals();
}

void BatchProgressWidget::initializeBatch(const QString &operationId, const QStringList &filePaths)
{
    if (operationId.isEmpty() || filePaths.isEmpty()) {
        return;
    }

    m_operationId = operationId;
    m_filePaths = filePaths;
    m_fileStatuses.clear();

    // Initialize file statuses (1-based indexing for display)
    for (int i = 0; i < filePaths.size(); ++i) {
        FileStatusInfo statusInfo(filePaths[i], i + 1);
        statusInfo.status = FileStatus::PENDING;
        m_fileStatuses.append(statusInfo);
    }

    // Clear and populate file list widget
    m_fileListWidget->clear();
    for (const auto &statusInfo : m_fileStatuses) {
        QListWidgetItem *item = new QListWidgetItem(m_fileListWidget);
        
        QString itemText = QString("%1. %2\n   %3")
            .arg(statusInfo.fileIndex)
            .arg(statusInfo.fileName)
            .arg(statusInfo.filePath);
        
        item->setText(itemText);
        item->setData(Qt::UserRole, statusInfo.fileIndex);
        item->setData(Qt::UserRole + 1, statusInfo.filePath);
        
        // Set initial styling (pending)
        item->setBackground(QBrush(QColor(250, 250, 250)));
        item->setForeground(QBrush(QColor(100, 100, 100)));
        
        m_fileListWidget->addItem(item);
    }

    // Update UI
    updateCurrentFileIndicator();
    updateOverallProgress();
    
    // Update title
    m_titleLabel->setText(tr("Batch Encryption Progress - %1 files").arg(filePaths.size()));
}

void BatchProgressWidget::onBatchEncryptionStarted(const QString &operationId)
{
    if (!validateOperationId(operationId)) {
        return;
    }

    // Update UI to show operation started
    m_currentFileLabel->setText(tr("Starting batch encryption..."));
    m_overallProgressBar->setValue(0);
}

void BatchProgressWidget::onBatchEncryptionProgress(const QString &operationId, int fileIndex, int percentage)
{
    if (!validateOperationId(operationId)) {
        return;
    }

    // Convert 0-based fileIndex to 1-based for display
    int displayIndex = fileIndex + 1;
    
    if (displayIndex < 1 || displayIndex > m_fileStatuses.size()) {
        return;
    }

    // Update file status
    FileStatusInfo &statusInfo = m_fileStatuses[displayIndex - 1];
    
    // If this is the first progress update for this file, mark it as in progress
    if (statusInfo.status == FileStatus::PENDING) {
        statusInfo.status = FileStatus::IN_PROGRESS;
    }
    
    statusInfo.progressPercentage = percentage;

    // Update UI
    updateFileStatus(displayIndex, statusInfo.status, percentage);
    updateCurrentFileIndicator();
    updateOverallProgress();
}

void BatchProgressWidget::onBatchEncryptionCompleted(const QString &operationId, const QString &status)
{
    if (!validateOperationId(operationId)) {
        return;
    }

    // Mark all pending files as completed (in case some weren't processed)
    for (auto &statusInfo : m_fileStatuses) {
        if (statusInfo.status == FileStatus::PENDING || statusInfo.status == FileStatus::IN_PROGRESS) {
            // If status is SUCCESS, mark as success; otherwise mark as failed
            if (status == "SUCCESS") {
                statusInfo.status = FileStatus::SUCCESS;
            } else {
                statusInfo.status = FileStatus::FAILED;
                statusInfo.errorMessage = tr("Batch operation failed");
            }
            updateFileStatus(statusInfo.fileIndex, statusInfo.status);
        }
    }

    // Update UI
    updateCurrentFileIndicator();
    updateOverallProgress();
    
    // Update title
    if (status == "SUCCESS") {
        m_titleLabel->setText(tr("Batch Encryption Complete - All files encrypted successfully"));
        m_currentFileLabel->setText(tr("All files processed successfully"));
    } else {
        m_titleLabel->setText(tr("Batch Encryption Complete - Some files failed"));
        m_currentFileLabel->setText(tr("Batch operation completed with errors"));
    }

    // Disable cancel button
    m_cancelButton->setEnabled(false);

    // Emit completion signal
    emit batchCompleted(operationId, status);
}

void BatchProgressWidget::onEncryptionError(const QString &operationId, const QString &errorMessage)
{
    if (!validateOperationId(operationId)) {
        return;
    }

    // Find the file that's currently in progress and mark it as failed
    for (auto &statusInfo : m_fileStatuses) {
        if (statusInfo.status == FileStatus::IN_PROGRESS) {
            statusInfo.status = FileStatus::FAILED;
            statusInfo.errorMessage = errorMessage;
            updateFileStatus(statusInfo.fileIndex, FileStatus::FAILED);
            break;
        }
    }

    updateCurrentFileIndicator();
    updateOverallProgress();
}

void BatchProgressWidget::onCancelClicked()
{
    // TODO: Implement cancellation logic (requires D-Bus method call)
    // For now, just show a message
    QMessageBox::information(this, tr("Cancel Operation"),
                            tr("Cancellation will be implemented in a future story."));
}

void BatchProgressWidget::onFileItemClicked(QListWidgetItem *item)
{
    if (!item) {
        return;
    }

    int fileIndex = item->data(Qt::UserRole).toInt();
    if (fileIndex < 1 || fileIndex > m_fileStatuses.size()) {
        return;
    }

    const FileStatusInfo &statusInfo = m_fileStatuses[fileIndex - 1];
    
    // If file failed, show error details
    if (statusInfo.status == FileStatus::FAILED && !statusInfo.errorMessage.isEmpty()) {
        QMessageBox::warning(this,
                            tr("Encryption Error - File %1").arg(fileIndex),
                            tr("File: %1\n\nError: %2")
                                .arg(statusInfo.filePath)
                                .arg(statusInfo.errorMessage));
    }
}

void BatchProgressWidget::updateOverallProgress()
{
    if (m_fileStatuses.isEmpty()) {
        m_overallProgressBar->setValue(0);
        return;
    }

    int totalProgress = 0;
    int fileCount = m_fileStatuses.size();

    for (const auto &statusInfo : m_fileStatuses) {
        switch (statusInfo.status) {
            case FileStatus::SUCCESS:
                totalProgress += 100;
                break;
            case FileStatus::IN_PROGRESS:
                totalProgress += statusInfo.progressPercentage;
                break;
            case FileStatus::FAILED:
                // Failed files count as 0% progress
                break;
            case FileStatus::PENDING:
                // Pending files count as 0% progress
                break;
        }
    }

    int overallPercentage = (fileCount > 0) ? (totalProgress / fileCount) : 0;
    m_overallProgressBar->setValue(overallPercentage);
}

void BatchProgressWidget::updateCurrentFileIndicator()
{
    if (m_fileStatuses.isEmpty()) {
        m_currentFileLabel->setText(tr("No files in batch"));
        return;
    }

    // Find current file being processed (IN_PROGRESS)
    int currentFileIndex = 0;
    int totalFiles = m_fileStatuses.size();

    for (const auto &statusInfo : m_fileStatuses) {
        if (statusInfo.status == FileStatus::IN_PROGRESS) {
            currentFileIndex = statusInfo.fileIndex;
            break;
        }
    }

    // If no file in progress, find the last completed file
    if (currentFileIndex == 0) {
        for (const auto &statusInfo : m_fileStatuses) {
            if (statusInfo.status == FileStatus::SUCCESS || statusInfo.status == FileStatus::FAILED) {
                currentFileIndex = statusInfo.fileIndex;
            }
        }
    }

    if (currentFileIndex > 0) {
        const FileStatusInfo &statusInfo = m_fileStatuses[currentFileIndex - 1];
        if (statusInfo.status == FileStatus::IN_PROGRESS) {
            m_currentFileLabel->setText(
                tr("Encrypting file %1 of %2: %3 (%4%)")
                    .arg(currentFileIndex)
                    .arg(totalFiles)
                    .arg(statusInfo.fileName)
                    .arg(statusInfo.progressPercentage));
        } else if (statusInfo.status == FileStatus::SUCCESS) {
            m_currentFileLabel->setText(
                tr("Completed file %1 of %2: %3")
                    .arg(currentFileIndex)
                    .arg(totalFiles)
                    .arg(statusInfo.fileName));
        } else if (statusInfo.status == FileStatus::FAILED) {
            m_currentFileLabel->setText(
                tr("Failed file %1 of %2: %3")
                    .arg(currentFileIndex)
                    .arg(totalFiles)
                    .arg(statusInfo.fileName));
        }
    } else {
        m_currentFileLabel->setText(tr("Preparing batch operation..."));
    }
}

void BatchProgressWidget::updateFileStatus(int fileIndex, FileStatus status, int progressPercentage)
{
    if (fileIndex < 1 || fileIndex > m_fileStatuses.size()) {
        return;
    }

    FileStatusInfo &statusInfo = m_fileStatuses[fileIndex - 1];
    statusInfo.status = status;
    if (progressPercentage >= 0) {
        statusInfo.progressPercentage = progressPercentage;
    }

    // Find corresponding list widget item
    for (int i = 0; i < m_fileListWidget->count(); ++i) {
        QListWidgetItem *item = m_fileListWidget->item(i);
        if (item && item->data(Qt::UserRole).toInt() == fileIndex) {
            // Update item text with status
            QString statusText = getStatusText(status);
            QString itemText = QString("%1. %2 [%3]\n   %4")
                .arg(fileIndex)
                .arg(statusInfo.fileName)
                .arg(statusText)
                .arg(statusInfo.filePath);
            
            if (status == FileStatus::IN_PROGRESS && progressPercentage > 0) {
                itemText += QString(" (%1%)").arg(progressPercentage);
            }
            
            item->setText(itemText);
            
            // Apply styling and xenon effects
            QString style = getStatusStyle(status);
            item->setBackground(QBrush(QColor(style)));
            applyXenonEffect(item, status);
            
            break;
        }
    }
}

void BatchProgressWidget::applyXenonEffect(QListWidgetItem *item, FileStatus status)
{
    // Xenon effects implementation
    // Note: Full xenon effects library integration will be added when available
    // For now, we apply visual styling that mimics xenon effects:
    
    QColor baseColor;
    QColor glowColor;
    
    switch (status) {
        case FileStatus::PENDING:
            baseColor = QColor(250, 250, 250);  // Light gray
            glowColor = QColor(200, 200, 200);
            break;
        case FileStatus::IN_PROGRESS:
            baseColor = QColor(240, 248, 255);  // Light blue (xenon glow)
            glowColor = QColor(74, 144, 226);   // Blue glow
            break;
        case FileStatus::SUCCESS:
            baseColor = QColor(240, 255, 248);  // Light green
            glowColor = QColor(80, 200, 120);    // Green glow
            break;
        case FileStatus::FAILED:
            baseColor = QColor(255, 240, 240);  // Light red
            glowColor = QColor(255, 100, 100);   // Red flash
            break;
    }
    
    // Apply base color
    item->setBackground(QBrush(baseColor));
    
    // TODO: When xenon effects library is available, integrate here:
    // For IN_PROGRESS: xenonEffects->applyGlow(item, glowColor);
    // For SUCCESS: xenonEffects->applyPulse(item, glowColor);
    // For FAILED: xenonEffects->applyFlash(item, glowColor);
    
    // For now, use a simple visual indicator
    if (status == FileStatus::IN_PROGRESS) {
        // Create a subtle animation effect (placeholder for xenon glow)
        item->setForeground(QBrush(glowColor));
    } else if (status == FileStatus::SUCCESS) {
        item->setForeground(QBrush(QColor(0, 150, 0)));
    } else if (status == FileStatus::FAILED) {
        item->setForeground(QBrush(QColor(200, 0, 0)));
    }
}

QString BatchProgressWidget::getStatusText(FileStatus status) const
{
    switch (status) {
        case FileStatus::PENDING:
            return tr("Pending");
        case FileStatus::IN_PROGRESS:
            return tr("In Progress");
        case FileStatus::SUCCESS:
            return tr("Success");
        case FileStatus::FAILED:
            return tr("Failed");
    }
    return tr("Unknown");
}

QString BatchProgressWidget::getStatusStyle(FileStatus status) const
{
    // Return color string (for compatibility with existing code)
    switch (status) {
        case FileStatus::PENDING:
            return "#FAFAFA";
        case FileStatus::IN_PROGRESS:
            return "#F0F8FF";
        case FileStatus::SUCCESS:
            return "#F0FFF8";
        case FileStatus::FAILED:
            return "#FFF0F0";
    }
    return "#FFFFFF";
}

void BatchProgressWidget::reset()
{
    m_operationId.clear();
    m_filePaths.clear();
    m_fileStatuses.clear();
    m_fileListWidget->clear();
    m_overallProgressBar->setValue(0);
    m_currentFileLabel->setText(tr("No batch operation active"));
    m_titleLabel->setText(tr("Batch Encryption Progress"));
    m_cancelButton->setEnabled(true);
}

bool BatchProgressWidget::validateOperationId(const QString &operationId) const
{
    // Security: Only process updates for the current operation
    return !m_operationId.isEmpty() && m_operationId == operationId;
}

bool BatchProgressWidget::connectToDBusSignals(const QString &serviceName, const QString &objectPath)
{
    // Disconnect existing connection if any
    disconnectFromDBusSignals();
    
    // Create D-Bus interface connection
    QDBusConnection connection = QDBusConnection::sessionBus();
    m_dbusInterface = new OrgMilosEncryptionManagerInterface(
        serviceName,
        objectPath,
        connection,
        this
    );
    
    // Connect to batch encryption signals
    connect(m_dbusInterface, &OrgMilosEncryptionManagerInterface::BatchEncryptionStarted,
            this, &BatchProgressWidget::onBatchEncryptionStarted);
    connect(m_dbusInterface, &OrgMilosEncryptionManagerInterface::BatchEncryptionProgress,
            this, &BatchProgressWidget::onBatchEncryptionProgress);
    connect(m_dbusInterface, &OrgMilosEncryptionManagerInterface::BatchEncryptionCompleted,
            this, &BatchProgressWidget::onBatchEncryptionCompleted);
    connect(m_dbusInterface, &OrgMilosEncryptionManagerInterface::EncryptionError,
            this, &BatchProgressWidget::onEncryptionError);
    
    return true;
}

void BatchProgressWidget::disconnectFromDBusSignals()
{
    if (m_dbusInterface) {
        disconnect(m_dbusInterface, nullptr, this, nullptr);
        m_dbusInterface->deleteLater();
        m_dbusInterface = nullptr;
    }
}

