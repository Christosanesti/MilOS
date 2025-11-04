#include "ui/mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QUrl>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application metadata
    app.setApplicationName("Quantum Encryption Manager");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("MilOS");
    
    // Parse command-line arguments for file paths
    QStringList filePaths;
    bool encryptMode = false;
    
    for (int i = 1; i < argc; ++i) {
        QString arg = QString::fromLocal8Bit(argv[i]);
        if (arg == "--encrypt" || arg == "-e") {
            encryptMode = true;
        } else if (arg.startsWith("--")) {
            // Skip other flags
            continue;
        } else {
            // Check if it's a file path
            QFileInfo fileInfo(arg);
            if (fileInfo.exists() && fileInfo.isFile()) {
                // Convert to absolute path
                filePaths << fileInfo.absoluteFilePath();
            } else {
                // Try as URL (from file manager)
                if (arg.startsWith("file://")) {
                    QString path = QUrl(arg).toLocalFile();
                    QFileInfo urlFileInfo(path);
                    if (urlFileInfo.exists() && urlFileInfo.isFile()) {
                        filePaths << urlFileInfo.absoluteFilePath();
                    }
                } else {
                    // Try as absolute or relative path
                    QFileInfo absFileInfo(QDir::currentPath() + "/" + arg);
                    if (absFileInfo.exists() && absFileInfo.isFile()) {
                        filePaths << absFileInfo.absoluteFilePath();
                    }
                }
            }
        }
    }
    
    // Create and show main window
    MainWindow window;
    
    // If files were provided, switch to batch mode and add them
    if (!filePaths.isEmpty()) {
        window.setFilesToEncrypt(filePaths);
        window.switchToBatchFileMode();
    }
    
    window.show();
    
    return app.exec();
}

