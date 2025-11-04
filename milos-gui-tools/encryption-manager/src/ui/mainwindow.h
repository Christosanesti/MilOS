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

private slots:
    /**
     * @brief Switch to single-file encryption mode
     */
    void switchToSingleFileMode();

    /**
     * @brief Switch to batch file encryption mode
     */
    void switchToBatchFileMode();

private:
    void setupUI();
    void setupMenuBar();
    void createDashboard();

    QStackedWidget *m_stackedWidget;
    QWidget *m_dashboardWidget;
    QWidget *m_singleFileWidget;
    QWidget *m_batchFileWidget;
    BatchFilePicker *m_batchFilePicker;
    QPushButton *m_singleFileButton;
    QPushButton *m_batchFileButton;
};

#endif // MAINWINDOW_H

