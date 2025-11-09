#ifndef HARDWAREDETECTIONSCREEN_H
#define HARDWAREDETECTIONSCREEN_H

#include <QObject>
#include <QQuickItem>

class HardwareDetectionScreen : public QQuickItem
{
    Q_OBJECT

public:
    explicit HardwareDetectionScreen(QQuickItem *parent = nullptr);

signals:
    void backClicked();
    void nextClicked();
};

#endif // HARDWAREDETECTIONSCREEN_H

