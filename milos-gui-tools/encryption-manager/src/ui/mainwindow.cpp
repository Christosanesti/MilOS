#include "mainwindow.h"
#include "batchfilepicker.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_stackedWidget(new QStackedWidget(this))
    , m_dashboardWidget(new QWidget(this))
    , m_singleFileWidget(new QWidget(this))
    , m_batchFileWidget(new QWidget(this))
    , m_batchFilePicker(new BatchFilePicker(this))
    , m_singleFileButton(new QPushButton(tr("Encrypt Single File"), this))
    , m_batchFileButton(new QPushButton(tr("Encrypt Multiple Files"), this))
{
    setWindowTitle(tr("Quantum Encryption Manager"));
    setMinimumSize(800, 600);

    setupMenuBar();
    setupUI();
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

