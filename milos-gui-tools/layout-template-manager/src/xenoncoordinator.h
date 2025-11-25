#ifndef XENONCOORDINATOR_H
#define XENONCOORDINATOR_H

#include <QObject>
#include <QStringList>

/**
 * @brief Xenon Effects Coordinator
 * 
 * Coordinates xenon effects across all monitors for unified visual theme
 */
class XenonCoordinator : public QObject
{
    Q_OBJECT

public:
    explicit XenonCoordinator(QObject* parent = nullptr);

    /**
     * @brief Synchronize xenon effects across all monitors
     * @return true if successful, false otherwise
     */
    bool synchronizeEffects();

    /**
     * @brief Apply unified visual theme across monitors
     * @return true if successful, false otherwise
     */
    bool applyUnifiedTheme();

signals:
    void effectsSynchronized(bool success);
    void error(const QString& errorMessage);

private:
    /**
     * @brief Get list of monitor identifiers
     * @return List of monitor IDs
     */
    QStringList getMonitorIds();

    /**
     * @brief Coordinate effect on specific monitor
     * @param monitorId Monitor identifier
     * @return true if successful, false otherwise
     */
    bool coordinateEffectOnMonitor(const QString& monitorId);
};

#endif // XENONCOORDINATOR_H

