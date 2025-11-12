#ifndef NETWORK_SEGMENTATION_H
#define NETWORK_SEGMENTATION_H

#include <QObject>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "segment_manager.h"
#include "topology_display.h"

/**
 * @brief Network Segmentation Application
 * 
 * Main application class for network segmentation tool.
 */
class NetworkSegmentation : public QObject {
    Q_OBJECT

public:
    explicit NetworkSegmentation(QObject* parent = nullptr);
    ~NetworkSegmentation();

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
    SegmentManager* m_segmentManager;
    TopologyDisplay* m_topologyDisplay;
};

#endif // NETWORK_SEGMENTATION_H

