#ifndef WINDOWGROUPER_H
#define WINDOWGROUPER_H

#include <QObject>
#include <QVariantMap>
#include <QStringList>

/**
 * @brief Window Grouper
 * 
 * Groups related windows together using KWin API
 */
class WindowGrouper : public QObject
{
    Q_OBJECT

public:
    explicit WindowGrouper(QObject* parent = nullptr);

    enum GroupType {
        TAB_GROUP,
        TILE_GROUP,
        STACK_GROUP
    };
    Q_ENUM(GroupType)

    /**
     * @brief Group windows based on template definition
     * @param template Template data with window grouping information
     * @return true if successful, false otherwise
     */
    bool groupWindows(const QVariantMap& template);

    /**
     * @brief Create a window group
     * @param groupId Group identifier
     * @param groupName Group name
     * @param applications List of application names in group
     * @param groupType Group type (TAB_GROUP, TILE_GROUP, STACK_GROUP)
     * @return true if successful, false otherwise
     */
    bool createGroup(const QString& groupId, const QString& groupName,
                    const QStringList& applications, GroupType groupType);

signals:
    void windowsGrouped(const QString& groupId, bool success);
    void error(const QString& errorMessage);

private:
    /**
     * @brief Find windows by application names
     * @param applications List of application names
     * @return List of window IDs
     */
    QStringList findWindows(const QStringList& applications);

    /**
     * @brief Group windows using KWin API
     * @param windowIds List of window IDs to group
     * @param groupType Group type
     * @return true if successful, false otherwise
     */
    bool groupWindowsViaKWin(const QStringList& windowIds, GroupType groupType);
};

#endif // WINDOWGROUPER_H

