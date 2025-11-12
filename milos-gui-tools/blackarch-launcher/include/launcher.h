#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QObject>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "tool_manager.h"

/**
 * @brief BlackArch Launcher Application
 * 
 * Main application class for unified BlackArch tool launcher.
 */
class Launcher : public QObject {
    Q_OBJECT

public:
    explicit Launcher(QObject* parent = nullptr);
    ~Launcher();

    /**
     * @brief Initialize launcher
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Run launcher
     * @return Application exit code
     */
    int run();

private:
    bool m_initialized;
    QApplication* m_app;
    QQmlApplicationEngine* m_engine;
    ToolManager* m_toolManager;
};

#endif // LAUNCHER_H

