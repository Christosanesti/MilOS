#ifndef ALERTZONEWIDGET_H
#define ALERTZONEWIDGET_H

#include <QObject>
#include <QQuickItem>

/**
 * @brief Alert Zone Widget C++ backend
 * 
 * Provides backend functionality for the alert zone Plasmoid widget.
 */
class AlertZoneWidget : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(int alertCount READ alertCount NOTIFY alertCountChanged)

public:
    explicit AlertZoneWidget(QQuickItem* parent = nullptr);
    ~AlertZoneWidget();

    int alertCount() const { return m_alertCount; }

public Q_SLOTS:
    Q_INVOKABLE void addAlert(const QString& severity, const QString& category, const QString& message);
    Q_INVOKABLE void clearAlerts();

Q_SIGNALS:
    void alertCountChanged(int count);

private:
    int m_alertCount;
};

#endif // ALERTZONEWIDGET_H

