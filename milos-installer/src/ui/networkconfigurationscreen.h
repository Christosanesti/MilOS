#ifndef NETWORKCONFIGURATIONSCREEN_H
#define NETWORKCONFIGURATIONSCREEN_H

#include <QObject>
#include <QQuickItem>

class NetworkConfigurationScreen : public QQuickItem
{
    Q_OBJECT

public:
    explicit NetworkConfigurationScreen(QQuickItem *parent = nullptr);

signals:
    void backClicked();
    void nextClicked();
    void skipClicked();
};

#endif // NETWORKCONFIGURATIONSCREEN_H

