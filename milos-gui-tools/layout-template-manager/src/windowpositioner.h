#ifndef WINDOWPOSITIONER_H
#define WINDOWPOSITIONER_H

#include <QObject>
#include <QVariantMap>
#include <QStringList>

/**
 * @brief Window Positioner
 * 
 * Positions windows on monitors using KWin scripting API
 */
class WindowPositioner : public QObject
{
    Q_OBJECT

public:
    explicit WindowPositioner(QObject* parent = nullptr);

    /**
     * @brief Position windows based on template definition
     * @param template Template data with window_positions
     * @return true if successful, false otherwise
     */
    bool positionWindows(const QVariantMap& template);

    /**
     * @brief Position a single window
     * @param applicationName Application identifier
     * @param monitorIndex Monitor index (0-based)
     * @param x X position
     * @param y Y position
     * @param width Window width
     * @param height Window height
     * @param maximized Whether window should be maximized
     * @return true if successful, false otherwise
     */
    bool positionWindow(const QString& applicationName, int monitorIndex,
                       int x, int y, int width, int height, bool maximized);

    /**
     * @brief Get monitor configuration
     * @return Monitor configuration map
     */
    QVariantMap getMonitorConfiguration();

signals:
    void windowPositioned(const QString& applicationName, bool success);
    void error(const QString& errorMessage);

private:
    /**
     * @brief Find window by application name
     * @param applicationName Application identifier
     * @return Window ID or empty string if not found
     */
    QString findWindow(const QString& applicationName);

    /**
     * @brief Get monitor geometry
     * @param monitorIndex Monitor index (0-based)
     * @return Monitor geometry map (x, y, width, height)
     */
    QVariantMap getMonitorGeometry(int monitorIndex);

    /**
     * @brief Use KWin scripting API to position window
     * @param windowId Window identifier
     * @param x X position
     * @param y Y position
     * @param width Window width
     * @param height Window height
     * @param maximized Whether window should be maximized
     * @return true if successful, false otherwise
     */
    bool positionWindowViaKWin(const QString& windowId, int x, int y,
                               int width, int height, bool maximized);
};

#endif // WINDOWPOSITIONER_H

