#ifndef SECURITY_STATUS_DASHBOARD_H
#define SECURITY_STATUS_DASHBOARD_H

#include <QObject>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "status_aggregator.h"

/**
 * @brief Security Status Dashboard Application
 * 
 * Main application class for security status dashboard.
 */
class SecurityStatusDashboard : public QObject {
    Q_OBJECT

public:
    explicit SecurityStatusDashboard(QObject* parent = nullptr);
    ~SecurityStatusDashboard();

    /**
     * @brief Initialize application
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Run application
     * @return Application exit code
     */
    int run();

private:
    bool m_initialized;
    QApplication* m_app;
    QQmlApplicationEngine* m_engine;
    StatusAggregator* m_statusAggregator;
};

#endif // SECURITY_STATUS_DASHBOARD_H

