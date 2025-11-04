#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMenuBar;
QT_END_NAMESPACE

class BatchFilePicker;
class BatchProgressWidget;
class QDialog;

/**
 * @brief Main window for Quantum Encryption Manager
 * 
 * This is the main application window that provides access to both
 * single-file and batch file encryption workflows.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    /**
     * @brief Set files to encrypt (from command-line arguments or service menu)
     * @param filePaths List of file paths to encrypt
     */
    void setFilesToEncrypt(const QStringList &filePaths);

private slots:
    /**
     * @brief Switch to single-file encryption mode
     */
    void switchToSingleFileMode();

    /**
     * @brief Switch to batch file encryption mode
     */
    void switchToBatchFileMode();

    /**
     * @brief Handle batch encryption started signal
     * @param operationId Operation ID
     */
    void onBatchEncryptionStarted(const QString &operationId);

    /**
     * @brief Handle batch encryption completed signal
     * @param operationId Operation ID
     * @param status Operation status
     */
    void onBatchCompleted(const QString &operationId, const QString &status);

private:
    void setupUI();
    void setupMenuBar();
    void createDashboard();
    void setupDBusConnection();

    QStackedWidget *m_stackedWidget;
    QWidget *m_dashboardWidget;
    QWidget *m_singleFileWidget;
    QWidget *m_batchFileWidget;
    BatchFilePicker *m_batchFilePicker;
    QPushButton *m_singleFileButton;
    QPushButton *m_batchFileButton;
    BatchProgressWidget *m_batchProgressWidget;
    QDialog *m_progressDialog;
};

#endif // MAINWINDOW_H

