#include "batchfilepicker.h"
#include <QFileDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileInfo>
#include <QMessageBox>
#include <QIcon>
#include <QSize>
#include <QFont>
#include <QPalette>
#include <QBrush>
#include <QColor>
#include <QDir>
#include <QAbstractItemView>

BatchFilePicker::BatchFilePicker(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QVBoxLayout(this))
    , m_buttonLayout(new QHBoxLayout())
    , m_selectFilesButton(new QPushButton(tr("Select Multiple Files"), this))
    , m_fileCountLabel(new QLabel(tr("0 of %1 files selected").arg(MAX_FILES), this))
    , m_fileListWidget(new QListWidget(this))
{
    // Setup main layout
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    m_mainLayout->setSpacing(10);

    // Setup button layout
    m_buttonLayout->addWidget(m_selectFilesButton);
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_fileCountLabel);

    // Add button layout to main layout
    m_mainLayout->addLayout(m_buttonLayout);

    // Setup file list widget
    m_fileListWidget->setMinimumHeight(200);
    m_fileListWidget->setSelectionMode(QAbstractItemView::NoSelection);
    m_mainLayout->addWidget(m_fileListWidget);

    // Connect signals
    connect(m_selectFilesButton, &QPushButton::clicked, this, &BatchFilePicker::selectFiles);

    // Initial state
    updateFileCountDisplay();
}

BatchFilePicker::~BatchFilePicker() = default;

QList<FileInfo> BatchFilePicker::getSelectedFiles() const
{
    return m_selectedFiles;
}

bool BatchFilePicker::areAllFilesValid() const
{
    for (const auto &fileInfo : m_selectedFiles) {
        if (!fileInfo.isValid) {
            return false;
        }
    }
    return true;
}

int BatchFilePicker::getFileCount() const
{
    return m_selectedFiles.size();
}

void BatchFilePicker::clearSelection()
{
    m_selectedFiles.clear();
    updateFileList();
    updateFileCountDisplay();
    emit selectionChanged(0);
}

void BatchFilePicker::selectFiles()
{
    // Calculate how many files can still be selected
    int remainingSlots = MAX_FILES - m_selectedFiles.size();
    if (remainingSlots <= 0) {
        QMessageBox::information(this, tr("Maximum Files Reached"),
                                  tr("You can only select up to %1 files.").arg(MAX_FILES));
        return;
    }

    // Open file dialog with multi-select
    QStringList selectedPaths = QFileDialog::getOpenFileNames(
        this,
        tr("Select Files for Encryption"),
        QDir::homePath(),
        tr("All Files (*)"),
        nullptr,
        QFileDialog::DontUseNativeDialog
    );

    if (selectedPaths.isEmpty()) {
        return;
    }

    // Add files up to the maximum
    int addedCount = 0;
    for (const QString &path : selectedPaths) {
        if (m_selectedFiles.size() >= MAX_FILES) {
            QMessageBox::warning(this, tr("Maximum Files Reached"),
                                 tr("Only the first %1 files were added.").arg(MAX_FILES));
            break;
        }

        // Check for duplicates
        bool isDuplicate = false;
        for (const auto &existingFile : m_selectedFiles) {
            if (existingFile.filePath == path) {
                isDuplicate = true;
                break;
            }
        }

        if (isDuplicate) {
            continue;
        }

        // Create file info and validate
        FileInfo fileInfo(path);
        if (!validateFile(path)) {
            emit fileValidationError(path, tr("File does not exist or is not readable"));
            continue;
        }

        m_selectedFiles.append(fileInfo);
        addedCount++;
    }

    if (addedCount > 0) {
        updateFileList();
        updateFileCountDisplay();
        emit selectionChanged(m_selectedFiles.size());
    }
}

void BatchFilePicker::removeFile(int index)
{
    if (index >= 0 && index < m_selectedFiles.size()) {
        m_selectedFiles.removeAt(index);
        updateFileList();
        updateFileCountDisplay();
        emit selectionChanged(m_selectedFiles.size());
    }
}

void BatchFilePicker::onRemoveButtonClicked()
{
    // Get sender button
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) {
        return;
    }

    // Get index from button property
    bool ok;
    int index = button->property("fileIndex").toInt(&ok);
    if (ok && index >= 0 && index < m_selectedFiles.size()) {
        removeFile(index);
    }
}

void BatchFilePicker::updateFileCountDisplay()
{
    int count = m_selectedFiles.size();
    m_fileCountLabel->setText(tr("%1 of %2 files selected").arg(count).arg(MAX_FILES));
    
    // Disable select button if at maximum
    m_selectFilesButton->setEnabled(count < MAX_FILES);
}

bool BatchFilePicker::validateFile(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    return fileInfo.exists() && fileInfo.isReadable() && fileInfo.isFile();
}

void BatchFilePicker::updateFileList()
{
    m_fileListWidget->clear();

    for (int i = 0; i < m_selectedFiles.size(); ++i) {
        const FileInfo &fileInfo = m_selectedFiles[i];

        // Create list item text
        QString itemText = QString("%1\n%2 (%3)")
            .arg(fileInfo.fileName, fileInfo.filePath, formatFileSize(fileInfo.fileSize));

        QListWidgetItem *item = new QListWidgetItem(m_fileListWidget);
        
        // Set item properties
        item->setData(Qt::UserRole, fileInfo.filePath);
        item->setData(Qt::UserRole + 1, i); // Store index
        
        // Add remove button
        QWidget *itemWidget = new QWidget();
        QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
        itemLayout->setContentsMargins(5, 2, 5, 2);
        
        QLabel *fileLabel = new QLabel(itemText, itemWidget);
        fileLabel->setWordWrap(true);
        fileLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        
        QPushButton *removeButton = new QPushButton(tr("Remove"), itemWidget);
        removeButton->setProperty("fileIndex", i);
        connect(removeButton, &QPushButton::clicked, this, &BatchFilePicker::onRemoveButtonClicked);
        
        itemLayout->addWidget(fileLabel);
        itemLayout->addWidget(removeButton);
        
        itemWidget->setLayout(itemLayout);
        
        // Set widget for item before setting size hint
        m_fileListWidget->setItemWidget(item, itemWidget);
        
        // Calculate and set item size hint after widget is set
        itemWidget->adjustSize();
        item->setSizeHint(itemWidget->sizeHint());

        // Apply styling based on validation status
        if (fileInfo.isValid) {
            // Valid file - normal styling (xenon effects will be applied separately)
            item->setBackground(QBrush(QColor(240, 248, 255))); // Light blue tint
        } else {
            // Invalid file - error styling
            item->setBackground(QBrush(QColor(255, 240, 240))); // Light red tint
        }
    }

    // Apply xenon effects after updating list
    applyXenonEffects();
}

QString BatchFilePicker::formatFileSize(qint64 bytes)
{
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;

    if (bytes >= GB) {
        return tr("%1 GB").arg(QString::number(bytes / double(GB), 'f', 2));
    } else if (bytes >= MB) {
        return tr("%1 MB").arg(QString::number(bytes / double(MB), 'f', 2));
    } else if (bytes >= KB) {
        return tr("%1 KB").arg(QString::number(bytes / double(KB), 'f', 2));
    } else {
        return tr("%1 bytes").arg(bytes);
    }
}

void BatchFilePicker::applyXenonEffects()
{
    // Xenon effects implementation
    // Note: Xenon effects library integration will be added when the library is available
    // For now, we apply visual feedback using Qt styling:
    
    // Apply visual styling to indicate selected files
    // This provides visual feedback similar to xenon effects
    for (int i = 0; i < m_fileListWidget->count(); ++i) {
        QListWidgetItem *item = m_fileListWidget->item(i);
        if (item) {
            // Get file info from item data
            QString filePath = item->data(Qt::UserRole).toString();
            bool isValid = false;
            
            // Check if file is valid
            for (const auto &fileInfo : m_selectedFiles) {
                if (fileInfo.filePath == filePath) {
                    isValid = fileInfo.isValid;
                    break;
                }
            }
            
            // Apply visual styling based on validation status
            if (isValid) {
                // Valid files: light blue tint (xenon-like glow effect)
                item->setBackground(QBrush(QColor(240, 248, 255)));
            } else {
                // Invalid files: light red tint (error indication)
                item->setBackground(QBrush(QColor(255, 240, 240)));
            }
        }
    }
    
    // TODO: When xenon effects library is available, integrate here:
    // xenonEffects->applyGlow(m_fileListWidget, validItems);
    // xenonEffects->applySelectionEffect(m_fileListWidget);
}

