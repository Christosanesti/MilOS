#include "ui/mainwindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application metadata
    app.setApplicationName("Quantum Encryption Manager");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("MilOS");
    
    // Create and show main window
    MainWindow window;
    window.show();
    
    return app.exec();
}

