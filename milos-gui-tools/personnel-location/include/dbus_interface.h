#ifndef PERSONNEL_LOCATION_DBUS_INTERFACE_H
#define PERSONNEL_LOCATION_DBUS_INTERFACE_H

#include "location_tracker.h"
#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QStringList>

class LocationTracker;

/**
 * @brief Personnel Location D-Bus Interface
 */
class PersonnelLocationDBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.PersonnelLocation")

public:
    explicit PersonnelLocationDBusInterface(QObject* parent = nullptr);
    ~PersonnelLocationDBusInterface();

    bool initialize();
    void setTracker(LocationTracker* tracker);

public slots:
    bool StartTracking(const QString& personnelId);
    bool StopTracking(const QString& personnelId);
    QVariantMap GetCurrentLocation(const QString& personnelId);
    QStringList GetLocationHistory(const QString& personnelId, const QString& startTime, const QString& endTime);
    bool IsTrackingEnabled();

private:
    LocationTracker* m_tracker;
};

#endif // PERSONNEL_LOCATION_DBUS_INTERFACE_H

