#ifndef SCREENLOCK_H
#define SCREENLOCK_H

#include <QObject>

class ScreenLock : public QObject
{
    Q_OBJECT

public:
    explicit ScreenLock(QObject *parent = nullptr);
    
    Q_INVOKABLE void lockScreen();

signals:
    void screenLocked();
    void error(const QString &errorMessage);

private:
    // KDE Plasma/KWin D-Bus interface and systemd-logind integration
    bool lockViaKWin();
    bool lockViaSystemdLogind();
};

#endif // SCREENLOCK_H

