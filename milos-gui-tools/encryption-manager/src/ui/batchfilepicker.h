#ifndef BATCHFILEPICKER_H
#define BATCHFILEPICKER_H

#include <QWidget>
#include <QStringList>
#include <QList>
#include <QFileInfo>

QT_BEGIN_NAMESPACE
class QPushButton;
class QListWidget;
class QLabel;
class QVBoxLayout;
class QHBoxLayout;
QT_END_NAMESPACE

/**
 * @brief File information structure for batch file operations
 */
struct FileInfo {
    QString filePath;
    QString fileName;
    qint64 fileSize;
    bool isValid;
    
    FileInfo() : fileSize(0), isValid(false) {}
    FileInfo(const QString &path) : filePath(path), isValid(false) {
        QFileInfo info(path);
        fileName = info.fileName();
        fileSize = info.size();
        isValid = info.exists() && info.isReadable();
    }
};

/**
 * @brief Batch file selection UI component for selecting up to 5 files
 * 
 * This component provides a multi-file selection interface for batch encryption operations.
 * It displays selected files with metadata and allows removing individual files.
 */
class BatchFilePicker : public QWidget
{
    Q_OBJECT

public:
    explicit BatchFilePicker(QWidget *parent = nullptr);
    ~BatchFilePicker();

    /**
     * @brief Get list of selected files
     * @return List of FileInfo structures
     */
    QList<FileInfo> getSelectedFiles() const;

    /**
     * @brief Check if all selected files are valid
     * @return True if all files are valid and accessible
     */
    bool areAllFilesValid() const;

    /**
     * @brief Get count of selected files
     * @return Number of selected files (0-5)
     */
    int getFileCount() const;

    /**
     * @brief Maximum number of files allowed for batch operations
     */
    static constexpr int MAX_FILES = 5;

    /**
     * @brief Clear all selected files
     */
    void clearSelection();

public slots:
    /**
     * @brief Open file selection dialog for multi-file selection
     */
    void selectFiles();

    /**
     * @brief Remove a file from the selection
     * @param index Index of file to remove
     */
    void removeFile(int index);

signals:
    /**
     * @brief Emitted when file selection changes
     * @param count Number of files selected
     */
    void selectionChanged(int count);

    /**
     * @brief Emitted when a file validation error occurs
     * @param filePath Path of invalid file
     * @param errorMessage Error description
     */
    void fileValidationError(const QString &filePath, const QString &errorMessage);

private slots:
    void onRemoveButtonClicked();
    void updateFileCountDisplay();

private:
    /**
     * @brief Validate file accessibility
     * @param filePath Path to validate
     * @return True if file exists and is readable
     */
    bool validateFile(const QString &filePath);

    /**
     * @brief Update the list widget display
     */
    void updateFileList();

    /**
     * @brief Format file size for display
     * @param bytes File size in bytes
     * @return Formatted string (e.g., "1.5 MB")
     */
    QString formatFileSize(qint64 bytes);

    /**
     * @brief Apply xenon visual effects to file list items
     */
    void applyXenonEffects();

    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_buttonLayout;
    QPushButton *m_selectFilesButton;
    QLabel *m_fileCountLabel;
    QListWidget *m_fileListWidget;
    QList<FileInfo> m_selectedFiles;
};

#endif // BATCHFILEPICKER_H

