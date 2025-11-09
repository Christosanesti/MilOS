#ifndef UPDATECHECKSCREEN_H
#define UPDATECHECKSCREEN_H

#include <QObject>
#include <QQuickItem>

class UpdateCheckScreen : public QQuickItem
{
    Q_OBJECT

public:
    explicit UpdateCheckScreen(QQuickItem *parent = nullptr);

signals:
    void backClicked();
    void nextClicked();
    void skipClicked();
};

#endif // UPDATECHECKSCREEN_H

