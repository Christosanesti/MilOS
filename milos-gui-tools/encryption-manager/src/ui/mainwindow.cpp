#include "mainwindow.h"
#include "batchfilepicker.h"
#include "batchprogresswidget.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QDialog>
#include <QtDBus/QDBusConnection>
#include <QUrl>
#include <QFileInfo>
#include "dbus/encryptionmanager_interface.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_stackedWidget(new QStackedWidget(this))
    , m_dashboardWidget(new QWidget(this))
    , m_singleFileWidget(new QWidget(this))
    , m_batchFileWidget(new QWidget(this))
    , m_batchFilePicker(new BatchFilePicker(this))
    , m_singleFileButton(new QPushButton(tr("Encrypt Single File"), this))
    , m_batchFileButton(new QPushButton(tr("Encrypt Multiple Files"), this))
    , m_batchProgressWidget(new BatchProgressWidget(this))
    , m_progressDialog(nullptr)
{
    setWindowTitle(tr("Quantum Encryption Manager"));
    setMinimumSize(800, 600);

    setupMenuBar();
    setupUI();
    setupDBusConnection();
    
    // Connect progress widget signals
    connect(m_batchProgressWidget, &BatchProgressWidget::batchCompleted,
            this, &MainWindow::onBatchCompleted);
}

MainWindow::~MainWindow() = default;

void MainWindow::setupMenuBar()
{
    QMenuBar *menuBar = this->menuBar();

    // File menu
    QMenu *fileMenu = menuBar->addMenu(tr("&File"));
    QAction *exitAction = fileMenu->addAction(tr("E&xit"));
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    // Mode menu
    QMenu *modeMenu = menuBar->addMenu(tr("&Mode"));
    QAction *singleFileAction = modeMenu->addAction(tr("&Single File Encryption"));
    QAction *batchFileAction = modeMenu->addAction(tr("&Batch File Encryption"));
    
    connect(singleFileAction, &QAction::triggered, this, &MainWindow::switchToSingleFileMode);
    connect(batchFileAction, &QAction::triggered, this, &MainWindow::switchToBatchFileMode);

    // Help menu
    QMenu *helpMenu = menuBar->addMenu(tr("&Help"));
    QAction *aboutAction = helpMenu->addAction(tr("&About"));
    connect(aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, tr("About"),
                          tr("Quantum Encryption Manager\n\n"
                             "MilOS Encryption Manager for quantum-resistant encryption operations."));
    });
}

void MainWindow::setupUI()
{
    // Create dashboard
    createDashboard();

    // Setup single-file widget
    // Note: Single-file encryption UI components will be implemented in future stories
    // For now, this maintains the workflow structure without breaking compatibility
    QVBoxLayout *singleFileLayout = new QVBoxLayout(m_singleFileWidget);
    QLabel *singleFileLabel = new QLabel(tr("Single File Encryption Mode\n\nThis mode will be implemented in a future story."), m_singleFileWidget);
    singleFileLabel->setAlignment(Qt::AlignCenter);
    singleFileLayout->addWidget(singleFileLabel);
    singleFileLayout->addStretch();

    // Setup batch-file widget
    QVBoxLayout *batchFileLayout = new QVBoxLayout(m_batchFileWidget);
    batchFileLayout->addWidget(m_batchFilePicker);
    batchFileLayout->setContentsMargins(10, 10, 10, 10);

    // Add widgets to stacked widget
    m_stackedWidget->addWidget(m_dashboardWidget);
    m_stackedWidget->addWidget(m_singleFileWidget);
    m_stackedWidget->addWidget(m_batchFileWidget);

    // Set dashboard as default
    m_stackedWidget->setCurrentWidget(m_dashboardWidget);

    // Set central widget
    setCentralWidget(m_stackedWidget);

    // Connect dashboard buttons
    connect(m_singleFileButton, &QPushButton::clicked, this, &MainWindow::switchToSingleFileMode);
    connect(m_batchFileButton, &QPushButton::clicked, this, &MainWindow::switchToBatchFileMode);
}

void MainWindow::createDashboard()
{
    QVBoxLayout *dashboardLayout = new QVBoxLayout(m_dashboardWidget);
    dashboardLayout->setContentsMargins(20, 20, 20, 20);
    dashboardLayout->setSpacing(20);

    // Title
    QLabel *titleLabel = new QLabel(tr("Quantum Encryption Manager"), m_dashboardWidget);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    dashboardLayout->addWidget(titleLabel);

    // Description
    QLabel *descLabel = new QLabel(
        tr("Select an encryption mode to begin:"), m_dashboardWidget);
    descLabel->setAlignment(Qt::AlignCenter);
    dashboardLayout->addWidget(descLabel);

    // Button container
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(20);

    // Single file button
    m_singleFileButton->setMinimumSize(200, 80);
    m_singleFileButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #4A90E2;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #357ABD;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #2E6DA4;"
        "}"
    );
    buttonLayout->addWidget(m_singleFileButton);

    // Batch file button
    m_batchFileButton->setMinimumSize(200, 80);
    m_batchFileButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #50C878;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #3FAF6B;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #2E8E57;"
        "}"
    );
    buttonLayout->addWidget(m_batchFileButton);

    buttonLayout->addStretch();
    dashboardLayout->addLayout(buttonLayout);
    dashboardLayout->addStretch();
}

void MainWindow::switchToSingleFileMode()
{
    m_stackedWidget->setCurrentWidget(m_singleFileWidget);
    setWindowTitle(tr("Quantum Encryption Manager - Single File Mode"));
}

void MainWindow::switchToBatchFileMode()
{
    m_stackedWidget->setCurrentWidget(m_batchFileWidget);
    setWindowTitle(tr("Quantum Encryption Manager - Batch File Mode"));
}

void MainWindow::setupDBusConnection()
{
    // Connect to D-Bus signals for batch encryption progress
    QDBusConnection connection = QDBusConnection::sessionBus();
    OrgMilosEncryptionManagerInterface *dbusInterface = new OrgMilosEncryptionManagerInterface(
        "org.milos.EncryptionManager",
        "/org/milos/EncryptionManager",
        connection,
        this
    );
    
    // Connect to batch encryption started signal
    connect(dbusInterface, &OrgMilosEncryptionManagerInterface::BatchEncryptionStarted,
            this, &MainWindow::onBatchEncryptionStarted);
}

void MainWindow::onBatchEncryptionStarted(const QString &operationId)
{
    // Get file paths from batch file picker
    QList<FileInfo> selectedFiles = m_batchFilePicker->getSelectedFiles();
    if (selectedFiles.isEmpty()) {
        return;
    }
    
    // Extract file paths
    QStringList filePaths;
    for (const auto &fileInfo : selectedFiles) {
        filePaths.append(fileInfo.filePath);
    }
    
    // Initialize progress widget with batch operation
    m_batchProgressWidget->initializeBatch(operationId, filePaths);
    
    // Create and show progress dialog
    if (!m_progressDialog) {
        m_progressDialog = new QDialog(this);
        m_progressDialog->setWindowTitle(tr("Batch Encryption Progress"));
        m_progressDialog->setModal(true);
        m_progressDialog->setMinimumSize(600, 500);
        
        QVBoxLayout *dialogLayout = new QVBoxLayout(m_progressDialog);
        dialogLayout->addWidget(m_batchProgressWidget);
        m_progressDialog->setLayout(dialogLayout);
    }
    
    // Show progress dialog
    m_progressDialog->show();
    m_progressDialog->raise();
    m_progressDialog->activateWindow();
}

void MainWindow::onBatchCompleted(const QString &operationId, const QString &status)
{
    Q_UNUSED(operationId);
    
    // Keep progress dialog visible for a moment, then allow user to close it
    // The dialog will remain visible until user closes it (as per AC7)
    // We could auto-close after a delay, but AC7 says "remains visible until batch operation completes or is cancelled"
    // So we'll keep it visible and let user close it manually
    
    // Update button text to indicate completion
    if (m_progressDialog && m_batchProgressWidget) {
        // The progress widget already updates its UI on completion
        // Dialog remains visible (user can close it manually)
    }
}

void MainWindow::setFilesToEncrypt(const QStringList &filePaths)
{
    if (filePaths.isEmpty()) {
        return;
    }
    
    // Switch to batch mode first
    switchToBatchFileMode();
    
    // Set files in batch picker
    m_batchFilePicker->setFiles(filePaths);
}

