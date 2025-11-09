#ifndef WELCOMESCREEN_H
#define WELCOMESCREEN_H

#include <QObject>
#include <QQuickItem>

class WelcomeScreen : public QQuickItem
{
    Q_OBJECT

public:
    explicit WelcomeScreen(QQuickItem *parent = nullptr);

signals:
    void installClicked();
    void liveModeClicked();
};

#endif // WELCOMESCREEN_H

