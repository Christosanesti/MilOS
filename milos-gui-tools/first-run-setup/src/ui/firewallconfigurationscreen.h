#ifndef FIREWALLCONFIGURATIONSCREEN_H
#define FIREWALLCONFIGURATIONSCREEN_H

#include <QObject>
#include <QQuickItem>

class FirewallConfigurationScreen : public QQuickItem
{
    Q_OBJECT

public:
    explicit FirewallConfigurationScreen(QQuickItem *parent = nullptr);

signals:
    void backClicked();
    void nextClicked();
};

#endif // FIREWALLCONFIGURATIONSCREEN_H

